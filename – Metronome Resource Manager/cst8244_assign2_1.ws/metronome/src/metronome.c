
struct ioattr_t;
#define IOFUNC_ATTR_T struct ioattr_t
struct metro_ocb;
#define IOFUNC_OCB_T struct metro_ocb

#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>
#include <sys/types.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

#define METRO_PULSE_CODE _PULSE_CODE_MINAVAIL         //0
#define PAUSE_PULSE_CODE (_PULSE_CODE_MINAVAIL +1) 	  //1
#define START_PULSE_CODE (_PULSE_CODE_MINAVAIL +2)    //2
#define STOP_PULSE_CODE  (_PULSE_CODE_MINAVAIL +3)    //3
#define QUIT_PULSE_CODE  (_PULSE_CODE_MINAVAIL +4)    //4
#define SET_PULSE_CODE   (_PULSE_CODE_MINAVAIL +5)    //5
#define METRO_ATTACH  "metronome"

typedef union {
	struct _pulse pulse;
	char msg[255];
}my_message_t;

struct DataTableRow{
	int tst;
	int tsb;
	int intervals;
	char pattern[16];
};

struct DataTableRow t[] = {
		{2, 4, 4, "|1&2&"},
		{3, 4, 6, "|1&2&3&"},
		{4, 4, 8, "|1&2&3&4&"},
		{5, 4, 10, "|1&2&3&4-5-"},
		{3, 8, 6, "|1-2-3-"},
		{6, 8, 6, "|1&a2&a"},
		{9, 8, 9, "|1&a2&a3&a"},
		{12, 8, 12, "|1&a2&a3&a4&a"}
};

struct Timer_Properties{
	double bps;
	double measure;
	double interval;
	double nano_sec;

}typedef timer_props_t;

struct Metronome_Properties {
	int bpm;
	int tst;
	int tsb;
}typedef metro_props_t;

struct Metronome {
	metro_props_t m_props;
	timer_props_t t_props;
}typedef Metronome_t;

typedef struct ioattr_t {
	iofunc_attr_t attr;
	int device;
} ioattr_t;

typedef struct metro_ocb{
	iofunc_ocb_t ocb;
	char buffer[50];
}metro_ocb_t;

int io_read(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);
int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle,void *extra);
void *metronome_thread(void*metronome);
int search_idx_table(Metronome_t * Metronome);
metro_ocb_t * metro_ocb_calloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *mtattr);
void metro_ocb_free(IOFUNC_OCB_T *mocb);

name_attach_t * attach; /* Namespace connection */
Metronome_t Metronome;
int server_coid;
char data[255];

int main(int argc, char *argv[]) {
	dispatch_t * dpp;
	metro_ocb_t *mocb;
	resmgr_io_funcs_t io_funcs;
	resmgr_connect_funcs_t conn_funcs;
	ioattr_t ioattr;
	pthread_attr_t thread_attrib;
	dispatch_context_t * ctp;
	int id;
	int i;

	int server_coid; /* Thread ID */

	if (argc != 4) {
		printf("ERROR: You must provide all required arguments.\n");
		exit(EXIT_FAILURE); /* termination */
	}

	mocb = calloc(1, sizeof(metro_ocb_t));

	mocb->ocb.offset = 0;

	iofunc_funcs_t metro_ocb_funcs = {
	_IOFUNC_NFUNCS, mocb, mocb, };

	Metronome.m_props.bpm = atoi(argv[1]);
	Metronome.m_props.tst = atoi(argv[2]);
	Metronome.m_props.tsb = atoi(argv[3]);

	if ((dpp = dispatch_create()) == NULL) {
		fprintf(stderr, "%s:  Unable to allocate dispatch context.\n", argv[0]);
		return (EXIT_FAILURE); /* termination */
	}

	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &conn_funcs, _RESMGR_IO_NFUNCS,
			&io_funcs);
	conn_funcs.open = io_open;
	io_funcs.read = io_read;
	io_funcs.write = io_write;
	iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);
	id = resmgr_attach(dpp, NULL, "/dev/local/metronome", _FTYPE_ANY, 0, &conn_funcs, &io_funcs, &ioattr);
	ctp = dispatch_context_alloc(dpp);
	pthread_attr_init(&thread_attrib);
	pthread_create(NULL, &thread_attrib, &metronome_thread, &Metronome);
	while (1) {
			if((ctp = dispatch_block(ctp))){
				dispatch_handler(ctp);
			}else
				printf("ERROR \n");
	}
	pthread_attr_destroy(&thread_attrib); /* destroy thread attributes */
	name_detach(attach, 0); /* detach from namespace, terminating connection */
	name_close(server_coid); /* close server connection from name_open */
	return EXIT_SUCCESS; /* Successful TERMINATION */
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, metro_ocb_t *mocb) {

	int index,i; /* Search index */
	int nb;

	if (data == NULL)
		return 0;
		for (i = 0; i < 8; i++) {
				if (t[i].tsb == Metronome.m_props.tsb
						&& t[i].tst == Metronome.m_props.tst) {
					index = i; /* return index if found */
				}
			}

		sprintf(data,
				"[metronome: %d beats/min, time signature: %d/%d, sec-per-interval: %.2f, nanoSecs: %.0lf]\n",
				Metronome.m_props.bpm, t[index].tst,
				t[index].tsb, Metronome.t_props.interval,
				Metronome.t_props.nano_sec);

	nb = strlen(data);

	if (mocb->ocb.offset == nb)
		return 0;
	nb = min(nb, msg->i.nbytes);
	_IO_SET_READ_NBYTES(ctp, nb);
	//Copy data into reply buffer.
	SETIOV(ctp->iov, data, nb);
	mocb->ocb.offset += nb;
	if (nb > 0)
		mocb->ocb.flags |= IOFUNC_ATTR_ATIME;

	return (_RESMGR_NPARTS(1));
}
int io_write(resmgr_context_t *ctp, io_write_t *msg, metro_ocb_t *mocb) {

	int nb = 0;

		if (msg->i.nbytes == ctp->info.msglen - (ctp->offset + sizeof(*msg))) {
			char *buf;
			char * pause_msg;
			char * set_msg;
			int i, small_integer = 0;
			buf = (char *) (msg + 1);
			if (strstr(buf, "pause") != NULL) { /* Check if client sent Pause request */
				for (i = 0; i < 2; i++) { /* check for both <cmd> <value> */
					pause_msg = strsep(&buf, " "); /* split the current string */
				}
				small_integer = atoi(pause_msg); /* convert the value to int */
				if (small_integer >= 1 && small_integer <= 9) {

					MsgSendPulse(server_coid, SchedGet(0, 0, NULL), /* send pulse Pause pulse code with value in seconds to pause */
					PAUSE_PULSE_CODE, small_integer);
				} else {
					printf("Integer is not between 1 and 9.\n");
				}

				} else if (strstr(buf, "quit") != NULL) { /* Check if request is to quit */
				MsgSendPulse(server_coid, SchedGet(0, 0, NULL),/* Send QUIT PULSE */
				QUIT_PULSE_CODE, small_integer);
			} else if (strstr(buf, "start") != NULL) { /* Check for START request */
				MsgSendPulse(server_coid, SchedGet(0, 0, NULL),
						START_PULSE_CODE, small_integer); /* Send Start pulse code */
			} else if (strstr(buf, "stop") != NULL) { /* Check for Stop request */
				MsgSendPulse(server_coid, SchedGet(0, 0, NULL), STOP_PULSE_CODE,
						small_integer); /* Send Stop pulse code */

			} else if (strstr(buf, "set") != NULL) { /* Check for set request */
				for (i = 0; i < 4; i++) { /*split string */
					set_msg = strsep(&buf, " "); /* start splitting string  <bpm> <tst> <tsb>*/

					if (i == 1) { /* First string BPM */
						Metronome.m_props.bpm = atoi(set_msg); /* set to new bpm */
					} else if (i == 2) {/* SECOND STRING TST */
						Metronome.m_props.tst = atoi(set_msg);/* set to new tst */
					} else if (i == 3) {/* THIRD STRING TSB */
						Metronome.m_props.tsb = atoi(set_msg);/* set to new tsb */
					}
				}

				MsgSendPulse(server_coid, SchedGet(0, 0, NULL), SET_PULSE_CODE,
						small_integer); /* Send Set pulse, value doesn't matter since new values are already set */

			} else {
				printf("\nInvalid Command\n"); /* if ever command is invalid */
				strcpy(data, buf);
			}

			nb = msg->i.nbytes;
		}

	_IO_SET_WRITE_NBYTES(ctp, nb);

	if (msg->i.nbytes > 0)
		mocb->ocb.flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

	return (_RESMGR_NPARTS(0));
}

int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle,
		void *extra) {

	if ((server_coid = name_open(METRO_ATTACH, 0)) == -1) { /* open namespace connection when file is opened */
		perror("ERROR - name_open failed - io_open() \n "); /* On error */
		return EXIT_FAILURE;
	}
	return (iofunc_open_default(ctp, msg, &handle->attr, extra));
}

void *metronome_thread(void * ta) {
	struct sigevent event; /* event, listening for pulse */
	struct itimerspec itime; /* timer specs */
	timer_t timer_id; /* timer id */
	my_message_t msg; /* message structure */
	int rcvid; /* return from msg receive */
	int index = 0; /* indexing */
	char * tp; /* pointer, going to be used to point to metronome table */
	int timer_status;/* could have been put in timer_props but meh */

	/* PHASE 1 */
	if ((attach = name_attach(NULL, METRO_ATTACH, 0)) == NULL) {/* attach namespace */
		printf("Error - name_attach - ./metronome.c \n");
		exit(EXIT_FAILURE);
	}
	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0,
			attach->chid,
			_NTO_SIDE_CHANNEL, 0);
	event.sigev_priority = SIGEV_PULSE_PRIO_INHERIT;
	event.sigev_code = METRO_PULSE_CODE;

	/* CREATE TIMER ON PULSE EVENT */
	timer_create(CLOCK_REALTIME, &event, &timer_id);
	int i;
	/* Search for current properties in table */
		for (i = 0; i < 8; i++) { /* search through table */
			if (t[i].tsb == Metronome.m_props.tsb
					&& t[i].tst == Metronome.m_props.tst) {
				index = i; /* return index if found */
			}
	 	}


	Metronome.t_props.bps = (double) 60 / Metronome.m_props.bpm;
		/* Beat per Measure */
		Metronome.t_props.measure = Metronome.t_props.bps * 2;
		Metronome.t_props.interval = Metronome.t_props.measure
				/ Metronome.m_props.tsb;
		Metronome.t_props.nano_sec = (Metronome.t_props.interval
				- (int) Metronome.t_props.interval) * 1e+9;

	itime.it_value.tv_sec = 1;
					itime.it_value.tv_nsec = 0;
					itime.it_interval.tv_sec = Metronome.t_props.interval;
					itime.it_interval.tv_nsec = Metronome.t_props.nano_sec;
					timer_settime(timer_id, 0, &itime, NULL);	tp = t[index].pattern;/* table pointer to indexes of pattern string */

	/* PHASE 2 */

	for (;;) {
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) { /* PULSE RECEIVED */
			switch (msg.pulse.code) {
			case METRO_PULSE_CODE: /* GENERAL PULSE SENT by timer */
				if (*tp == '|') { /* check if first char | */
					printf("%.2s", tp); /* print first 2 chars */
					tp = (tp + 2);/* pointer now starts at 3 */
				} else if (*tp == '\0') { /* check if end of string*/
					printf("\n");
					tp = t[index].pattern;/* reset to beginning */
				} else {
					printf("%c", *tp++);
				}

				break;
			case PAUSE_PULSE_CODE: /* PAUSE PULSE */
				if (timer_status == 0) {
					itime.it_value.tv_sec = msg.pulse.value.sival_int; /*Pause timer for specified amount of seconds */
					timer_settime(timer_id, 0, &itime, NULL); /* set timer to pause */
				}
				break;
			case QUIT_PULSE_CODE: /* QUIT PULSE */
				timer_delete(timer_id); /* delete timer */
				name_detach(attach, 0); /* terminate namespace, close channel */
				name_close(server_coid); /* close server connection */
				exit(EXIT_SUCCESS); /* Terminate Program */
			case SET_PULSE_CODE: /* SET PULSE */
				for (i = 0; i < 8; i++) { /* search through table */
						if (t[i].tsb == Metronome.m_props.tsb
								&& t[i].tst == Metronome.m_props.tst) {
							index = i; /* return index if found */
						}
					}
				tp = t[index].pattern; /* point to new pattern */
				Metronome.t_props.bps = (double) 60 / Metronome.m_props.bpm;
				/* Beat per Measure */
				Metronome.t_props.measure = Metronome.t_props.bps * 2;
				Metronome.t_props.interval = Metronome.t_props.measure
						/ Metronome.m_props.tsb;
				Metronome.t_props.nano_sec = (Metronome.t_props.interval
						- (int) Metronome.t_props.interval) * 1e+9;

					itime.it_value.tv_sec = 1;
					itime.it_value.tv_nsec = 0;
					itime.it_interval.tv_sec = Metronome.t_props.interval;
					itime.it_interval.tv_nsec = Metronome.t_props.nano_sec;
					timer_settime(timer_id, 0, &itime, NULL);
				printf("\n");/* start on new line */
				break;
			case START_PULSE_CODE: /* START PULSE */
				if (timer_status == 1) {
					itime.it_value.tv_sec = 1;
					itime.it_value.tv_nsec = 0;
					itime.it_interval.tv_sec = Metronome.t_props.interval;
					itime.it_interval.tv_nsec = Metronome.t_props.nano_sec;
					timer_settime(timer_id, 0, &itime, NULL);
					timer_status = 0;
				}
				break;
			case STOP_PULSE_CODE: /* STOP PULSE */
				if (timer_status == 0) {
					itime.it_value.tv_sec = 0;
					timer_settime(timer_id, 0, &itime, NULL);
					timer_status = 1;
				}
				break;
			}
		}
		fflush(stdout);

	}
	return NULL;
}



