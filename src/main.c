#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <ffi.h>
#include "flog.h"

#define MAGIC 0x676f6c66

extern char _rodata_start, _rodata_end;
char *rodata_start = &_rodata_start;
char *rodata_end = &_rodata_end;

char *mmbuf;
int mbuf_size;
enum {
	MODE_BINARY,
	MODE_FPRINTF,
	MODE_SPRINTF,
	MODE_DPRINTF,
};

int decode (int fdin) {
	
	    
	    
		flog_msg_t *m = (void *)mmbuf;
	ffi_type *args[34] = {
		[0]		= &ffi_type_sint,
		[1]		= &ffi_type_pointer,
		[2 ... 33]	= &ffi_type_slong
	};
	void *values[34];
	ffi_cif cif;
	ffi_arg rc;
	size_t i, ret;
	char *fmt;
	int fdout=STDOUT_FILENO;

	values[0] = (void *)&fdout;

	while (1) {
		ret = read(fdin, mmbuf, sizeof(flog_msg_t));
		//printf("flog decoder: read %ld bytes\n", ret);
		
		//printf("version=%d, size=%d, nargs=%d, mask=%d, fmt=%ld, args[0]=%ld\n",m->version, m->size, m->nargs, m->mask, m->fmt, m->args[0]);

		
		
		if (ret == 0)
			break;
		if (ret < 0) {
			fprintf(stderr, "Unable to read a message: %m");
			return -1;
		}
		if (m->magic != MAGIC) {
			fprintf(stderr, "The log file was not properly closed\n");
			break;
		}
		ret = m->size - sizeof(flog_msg_t);
		//printf("[ret=%ld] = [m->size=%d] - [sizeof(m)=%ld]\n",ret,m->size, sizeof(flog_msg_t));
		if (m->size > mbuf_size) {
			fprintf(stderr, "The buffer is too small");
			return -1;
		}
		if (read(fdin, mmbuf + sizeof(flog_msg_t), ret) != ret) {
			fprintf(stderr, "Unable to read a message: %m");
			return -1;
		}

		fmt = mmbuf + m->fmt;
		//printf("fmt=%s\n",fmt);
		values[1] = &fmt;

		for (i = 0; i < m->nargs; i++) {
			values[i + 2] = (void *)&m->args[i];
			//printf("%ld-th arg, mask is %d, m->mask & (1u << i) is %d\n", i, m->mask, m->mask & (1u << i));
			if (m->mask & (1u << i)) {
				// string
				m->args[i] = (long)(mmbuf + m->args[i]);
				args[i+2] = &ffi_type_pointer;
				printf("%s",(char*)m->args[i]);
			}
			else {
				args[i+2] = &ffi_type_slong;
				//printf("long value is %ld\n",* ((long*)values[i+2]));
				
			}
		}

		if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, m->nargs + 2,
				 &ffi_type_sint, args) == FFI_OK)
		{	//printf("FFI PREP OK\n");
			ffi_call(&cif, FFI_FN(dprintf), &rc, values);
		}
		//else printf("FFI PREP not OK\n");
	    
	}
	return 0;
	
	
}

int main(int argc, char *argv[])
{

	int fdin = STDIN_FILENO;
	bool use_reader = false;
	struct stat st;
	int opt, idx;
	
	if (argc==1) goto usage;

	static const char short_opts[] = "hr:";
	static struct option long_opts[] = {
		{ "help",		no_argument,		0, 'h'	},
		{ "read",       required_argument,  0, 'r'  },
		{ },
	};

	while (1) {
		idx = -1;
		opt = getopt_long(argc, argv, short_opts, long_opts, &idx);
		if (opt == -1)
			break;

		switch (opt) {
		case 'r':
		    use_reader = true;
		    fdin = open(optarg, O_RDONLY, 0644);
			if (fdin < 0) {
				fprintf(stderr, "Can't open %s: %s\n",
					optarg, strerror(errno));
				exit(1);			
			}
			stat(optarg, &st);
			mbuf_size = st.st_size;
			if (!mbuf_size)  {
				fprintf(stderr, "Binlog file %s has zero length\n",
					optarg);
				exit(0);			
			}
			else {
				mmbuf=malloc(mbuf_size);
				if (!mmbuf)   {
					fprintf(stderr, "Can't allocate %d bytes in memory for binlog file %s\n",
					mbuf_size, optarg);
					exit(1);
				}
			}
			break;
		case 'h':
		default:
			goto usage;
		}
	}
	if (use_reader) {
		//printf("decoding file with descriptor %d \n",fdin);
		decode(fdin);
		return 0;
	}

	return 0;
	
usage:
	fprintf(stderr,
		"Reading binary log created by CRIU\n"
		"usage:\n"
		"flog_reader --read filename\n"
		);
	return 1;
}
