#include <stdio.h>
#include <assert.h>

#define PRINT_HEADER	35
#define LEADING_SPACES	"                                    \t"
#define BLANK_CYCLE	"   "
#define	FETCH_CYCLE	"IF "
#define	DISPATCH_CYCLE	"ID "
#define	ISSUE_CYCLE	"IS "
#define	EXECUTE_CYCLE	"EX "
#define	WRITEBACK_CYCLE	"WB "

#define ADDMAX	50


class printline {
	private:
		FILE *fp;
		unsigned int lineno;
		unsigned int min_cycle;
		unsigned int max_cycle;
		unsigned int base_cycle;

		void print_header() {
		   base_cycle = min_cycle;

		   fprintf(fp, LEADING_SPACES);
		   for (unsigned int i = min_cycle; i < max_cycle + ADDMAX; i++)
		      fprintf(fp, "%d  ", i/1000);
		   fprintf(fp, "\n");

		   fprintf(fp, LEADING_SPACES);
		   for (unsigned int i = min_cycle; i < max_cycle + ADDMAX; i++)
		      fprintf(fp, "%d  ", i/100 - ((i/1000) * 10));
		   fprintf(fp, "\n");

		   fprintf(fp, LEADING_SPACES);
		   for (unsigned int i = min_cycle; i < max_cycle + ADDMAX; i++)
		      fprintf(fp, "%d  ", i/10 - ((i/100) * 10));
		   fprintf(fp, "\n");

		   fprintf(fp, LEADING_SPACES);
		   for (unsigned int i = min_cycle; i < max_cycle + ADDMAX; i++)
		      fprintf(fp, "%d  ", i - ((i/10) * 10));
		   fprintf(fp, "\n");
		}

	public:
		printline(FILE *fp) {
		   this->fp = fp;
		   this->lineno = 0;
		   this->min_cycle = 0;
		   this->max_cycle = 100;

#if 0
		   if (n_cycles > 10000) {
		      fprintf(stderr, "You are asking to view more than 10000 cycles (%d), this is not supported, exiting...\n", n_cycles);
		      fprintf(stderr, "If you cannot determine the problem, contact `ericro@ece.ncsu.edu'.\n");
		      exit(-1);
		   }
#endif
		}

		~printline() {
		}

		void print(char *line) {
		   unsigned int scan;

		   unsigned int seq_no;
		   unsigned int fu_type;
		   int src1, src2, dst;
		   unsigned int if_cycle, if_dur;
		   unsigned int id_cycle, id_dur;
		   unsigned int is_cycle, is_dur;
		   unsigned int ex_cycle, ex_dur;
		   unsigned int wb_cycle, wb_dur;
		   unsigned int i, cycle;

		   // Print header every so often...
		   if ((lineno % PRINT_HEADER) == 0)
		      print_header();

		   scan = sscanf(line, "%d fu{%d} src{%d,%d} dst{%d} IF{%d,%d} ID{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d}",
			&seq_no,
			&fu_type,
			&src1, &src2,
			&dst,
			&if_cycle, &if_dur,
			&id_cycle, &id_dur,
			&is_cycle, &is_dur,
			&ex_cycle, &ex_dur,
			&wb_cycle, &wb_dur);
		   if (scan != 15) {
		      fprintf(stderr, "Error parsing line %d, exiting...\n",
					lineno);
		      fprintf(stderr, "Here's what I read in: `%s'.\n", line);
		      fprintf(stderr, "If you cannot determine the problem, contact `ericro@ece.ncsu.edu'.\n");
		      exit(-1);
		   }

		   fprintf(fp, "%8d fu{%d} src{%3d,%3d} dst{%3d}\t",
				seq_no, fu_type, src1, src2, dst);

		   //////////////////////////////////////////////////////
		   // Check consistency of cycle/duration information.
		   //////////////////////////////////////////////////////
		   if (if_cycle < min_cycle) {
		      fprintf(stderr, "Line %d: `IF cycle (%d)' is inconsistent with (i.e., less than) previous fetch cycles, exiting...\n",
			lineno, if_cycle);
		      fprintf(stderr, "If you cannot determine the problem, contact `ericro@ece.ncsu.edu'.\n");
		      exit(-1);
		   }
		   else {
		      min_cycle = if_cycle;
		   }

		   if (id_cycle != (if_cycle + if_dur)) {
		      fprintf(stderr, "Line %d: `ID cycle (%d)' is inconsistent with `IF cycle (%d)' and `IF duration (%d)', exiting...\n",
			lineno, id_cycle, if_cycle, if_dur);
		      fprintf(stderr, "If you cannot determine the problem, contact `ericro@ece.ncsu.edu'.\n");
		      exit(-1);
		   }
		   else if (is_cycle != (id_cycle + id_dur)) {
		      fprintf(stderr, "Line %d: `IS cycle (%d)' is inconsistent with `ID cycle (%d)' and `ID duration (%d)', exiting...\n",
			lineno, is_cycle, id_cycle, id_dur);
		      fprintf(stderr, "If you cannot determine the problem, contact `ericro@ece.ncsu.edu'.\n");
		      exit(-1);
		   }
		   else if (ex_cycle != (is_cycle + is_dur)) {
		      fprintf(stderr, "Line %d: `EX cycle (%d)' is inconsistent with `IS cycle (%d)' and `IS duration (%d)', exiting...\n",
			lineno, ex_cycle, is_cycle, is_dur);
		      fprintf(stderr, "If you cannot determine the problem, contact `ericro@ece.ncsu.edu'.\n");
		      exit(-1);
		   }
		   else if (wb_cycle != (ex_cycle + ex_dur)) {
		      fprintf(stderr, "Line %d: `WB cycle (%d)' is inconsistent with `EX cycle (%d)' and `EX duration (%d)', exiting...\n",
			lineno, wb_cycle, ex_cycle, ex_dur);
		      fprintf(stderr, "If you cannot determine the problem, contact `ericro@ece.ncsu.edu'.\n");
		      exit(-1);
		   }

		   if (max_cycle < (wb_cycle + wb_dur))
		      max_cycle = (wb_cycle + wb_dur);

		   //////////////////////////
		   // Leading blank cycles.
		   //////////////////////////
		   //for (i = 0; i < if_cycle; i++)
		   //   fprintf(fp, BLANK_CYCLE);

		   assert(base_cycle <= if_cycle);
		   for (i = base_cycle; i < if_cycle; i++)
		      fprintf(fp, BLANK_CYCLE);

		   // additional error checking
		   cycle = if_cycle;

		   //////////////////////////
		   // IF stage
		   //////////////////////////
		   for (i = 0; i < if_dur; i++)
		      fprintf(fp, FETCH_CYCLE);

		   // additional error checking
		   cycle += if_dur;
		   assert(cycle == id_cycle);

		   //////////////////////////
		   // ID stage
		   //////////////////////////
		   for (i = 0; i < id_dur; i++)
		      fprintf(fp, DISPATCH_CYCLE);

		   // additional error checking
		   cycle += id_dur;
		   assert(cycle == is_cycle);

		   //////////////////////////
		   // IS stage
		   //////////////////////////
		   for (i = 0; i < is_dur; i++)
		      fprintf(fp, ISSUE_CYCLE);

		   // additional error checking
		   cycle += is_dur;
		   assert(cycle == ex_cycle);

		   //////////////////////////
		   // EX stage
		   //////////////////////////
		   for (i = 0; i < ex_dur; i++)
		      fprintf(fp, EXECUTE_CYCLE);

		   // additional error checking
		   cycle += ex_dur;
		   assert(cycle == wb_cycle);

		   //////////////////////////
		   // WB stage
		   //////////////////////////
		   for (i = 0; i < wb_dur; i++)
		      fprintf(fp, WRITEBACK_CYCLE);

		   // Go to next line.
		   fprintf(fp, "\n");
		   lineno++;
		}
};
