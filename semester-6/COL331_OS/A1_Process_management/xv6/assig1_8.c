
#include "types.h"
#include "stat.h"
#include "user.h"
#define NUM_HELPER 7
#define FILE_SIZE 1000

void itoa1(int j, char *c, int sz)
{
	char out1[8];
	for (int i = 0; i < sz; i++)
	{
		out1[sz - 1 - i] = '0' + j % 10;
		j /= 10;
	}
	strcpy(c, out1);
}
int stoi(char *c, int sz)
{
	int ans = 0;

	for (int i = 0; i < sz; i++)
	{
		int j = c[i] - '0';
		ans *= 10;
		ans += j;
	}
	return ans;
}

void fts(float f, char *c, int sz)
{

	char out1[8];
	int integerpart = (int)f;
	float fracpart = f - integerpart;

	int ind = 0;

	if (integerpart == 0)
	{
		out1[0] = '0';
		ind += 1;
	}
	else
	{
		while (integerpart > 0)
		{
			out1[ind] = '0' + integerpart % 10;
			ind++;
			integerpart /= 10;
		}
		for (int i = 0; i < ind / 2; i++)
		{
			char temp = out1[i];
			out1[i] = out1[ind - i - 1];
			out1[ind - i - 1] = temp;
		}
	}

	out1[ind] = '.';
	ind++;
	while (ind < 8)
	{
		fracpart *= 10;
		int j = (int)fracpart;
		out1[ind] = '0' + j;
		ind++;
		fracpart -= j;
	}
	strcpy(c, out1);
	// printf(1  , "Float is %s" , out1) ;
}

float stof(char *c)
{
	int intpart = 0;
	float floatpart = 0.0;
	int ind = 0;
	while (c[ind] != '.' && ind < 8)
	{
		intpart *= 10;
		intpart += c[ind] - '0';
		ind++;
	}
	// printf(1 , "the int part is %d \n" , intpart);
	ind++;
	float divider = 10;
	while (ind < 8)
	{
		floatpart += ((float)(c[ind] - '0')) / divider;
		divider *= 10;
		ind++;
	}
	char checkingagain[8];

	fts(floatpart, checkingagain, 8);
	floatpart += (float)intpart;
	return floatpart;
}


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf(1, "Need type and input filename\n");
		exit();
	}
	char *filename;
	filename = argv[2];
	int type = atoi(argv[1]);
	printf(1, "Type is %d and filename is %s\n", type, filename);

	int size = 1000;
	short arr[size];
	char c;
	int fd = open(filename, 0);

	for (int i = 0; i < size; i++)
	{
		read(fd, &c, 1);
		arr[i] = c - '0';
		read(fd, &c, 1);
	}
	close(fd);
	// this is to supress warning
	// printf(1, "%d elem %d\n", 0, arr[0]);

	//----FILL THE CODE HERE for unicast sum

	//------------------
	int parent_pid = getpid();

	int curr = 0;
	char sums[NUM_HELPER][9];
	int final_answer = 0;
	int child_pids[NUM_HELPER];
	for (int proc = 1; proc <= NUM_HELPER; proc++)
	{
		curr = proc;
		int curpid = fork();
		if (curpid == 0)
		{
			goto child;
		}
		child_pids[proc - 1] = curpid;
		int recieved = -1;

		while (recieved == -1)
		{
			recieved = recv(sums[proc - 1]);
		}
	}
	for (int i = 0; i < NUM_HELPER; i++)
	{
		final_answer += stoi(sums[i], 8);
	}
	printf(1, "Sum of array for file %s is %d\n", filename, final_answer);

	// exit();
	goto unimulti;
child: // send to server
	int sum = 0;
	for (int ind = curr - 1; ind < 1000; ind += NUM_HELPER)
	{
		// making add syscall
		sum = add(sum, arr[ind]);
	}

	char int_string[8];
	itoa1(sum, int_string, 8);
	
	send(getpid(), parent_pid, int_string);

	// exit child
	// exit();
	goto unimulti;

unimulti:
	if (type == 0)
	{
		exit();
	}
	else if (type == 1)
	{
		if (getpid() == parent_pid)
		{
			float mean = ((float)final_answer) / FILE_SIZE;
			// printf(1, "mean is : %d\n" ,final_answer) ;
			char mean_str[8];
			fts(mean, mean_str, 8);
			send_multi(parent_pid, child_pids, mean_str);
			float variance = 0.0 ;
			char variance_s[8] ;
			for(int rec_ind = 0 ; rec_ind < NUM_HELPER ; rec_ind++)
			{
				int ch = -1; 
				char s_var[8];
				float var = 0.0 ;
				while (ch ==-1)
				{
					ch = recv(s_var) ; 
				}
				var = stof(s_var) ;
				variance += var ; 
			}
			float final_var = variance / FILE_SIZE ; 
			fts(final_var , variance_s , 8) ;
			printf(1, "Variance of array for file %s is %s\n", filename, variance_s);

			printf(1, "The final variance is %s \n" , variance_s);
			
		}
		else
		{
			int st = -1;
			char msg[8];
			while (st == -1)
			{

				st = recv(msg);
			}
			float f_mean = stof(msg);

			float curvar = 0.0;
			for (int ind = curr - 1; ind < 1000; ind += NUM_HELPER)
			{
				float diff = (float)arr[ind] - f_mean ; 
				curvar += diff * diff ; 
				
			}
			char curvar_s[8] ;
			fts(curvar  , curvar_s , 8); 
			printf(1 , "The variance sent by the child %d is %s\n"  , curr , curvar_s);
			send(getpid() , parent_pid ,curvar_s ) ; 
		}
	}
	else
	{
		printf(1, "The value of first argument is out of range!\n");
	}
	exit();
}