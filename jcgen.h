#ifndef JCGEN_H
#define JCGEN_H

#include<cstdio>
#include<cstring>
#include<ctime>
#include<cstdlib>
#include<random>
#include<algorithm>

#include<io.h>
#include<windows.h>

namespace JCtool{

/*
颜色属性由两个十六进制数字指定 -- 第一个
对应于背景，第二个对应于前景。每个数字
可以为以下任何值:

0 = 黑色
1 = 蓝色
2 = 绿色
3 = 湖蓝色
4 = 红色
5 = 紫色
6 = 黄色
7 = 白色
8 = 灰色
9 = 亮蓝色
A = 亮绿色
B = 亮湖蓝色
C = 亮红色
D = 亮紫色
E = 亮黄色
F = 亮白色 
*/
const int default_color=0x07;

void set_color(const unsigned int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color);
	return;
}
void reset_color()
{
	set_color(default_color);
	return;
}
void color_print(const std::string content,const int color)
{
	set_color(color);
	printf("%s",content.c_str());
	set_color(default_color);
	return;
}

void print_info(const std::string &info)
{
	reset_color();
	printf("%s\n",info.c_str());
	return;
}
void print_success(const std::string &info)
{
	printf("[");
	JCtool::color_print("Success",0x0A);
	printf("]");
	printf("%s\n",info.c_str());
	return;
}
void print_warning(const std::string &info)
{
	printf("[");
	JCtool::color_print("Warning",0x06);
	printf("]");
	printf("%s\n",info.c_str());
	return;
}
void print_error(const std::string &info)
{
	printf("[");
	JCtool::color_print("ERROR",0x04);
	printf("]");
	printf("%s\n",info.c_str());
	return;
}

}

namespace JCrand{

std::default_random_engine engine;

bool is_initialized=false;
void Init_engine()
{
	unsigned int initial_time=std::time(nullptr);
	std::random_device rd_seed;
	srand(initial_time+rd_seed());
	engine.seed(rand());
	is_initialized=true;
	return;
}
void check_init()
{
	if(!is_initialized) Init_engine();
	return;
}

int random(const int l,const int r)
{
	check_init();
	std::uniform_int_distribution<int> generator(l,r);
	return generator(engine);
}
unsigned int random_u(const unsigned int l,const unsigned int r)
{
	check_init();
	std::uniform_int_distribution<unsigned int> generator(l,r);
	return generator(engine);
}
long long random_ll(const long long l,const long long r)
{
	check_init();
	std::uniform_int_distribution<long long> generator(l,r);
	return generator(engine);
}
unsigned long long random_ull(const unsigned long long l,const unsigned long long r)
{
	check_init();
	std::uniform_int_distribution<unsigned long long> generator(l,r);
	return generator(engine);
}

const std::string s_lower_case="abcdefghijklmnopqrstuvwxyz";
const std::string s_upper_case="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string s_number="0123456789";
const std::string s_special="!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
const std::string s_universal=s_lower_case+s_upper_case+s_number+s_special+'\n';
char random_c(const std::string &src=s_universal)
{
	check_init();
	std::uniform_int_distribution<int> generator(0,src.size()-1);
	return src[generator(engine)];
}
std::string random_s(const int len,const std::string &src=s_universal)
{
	check_init();
	std::uniform_int_distribution<int> generator(0,src.size()-1);
	std::string res; res.clear();
	for(int i=0;i<len;i++)
		res.push_back(src[generator(engine)]);
	return res;
}

bool random_b()
{
	check_init();
	return engine()&1u;
}

double random_f(const double l=0,const double r=1)
{
	check_init();
	std::uniform_real_distribution<double> urd(l,r);
	return urd(engine);
}
long double random_F(const long double l=0,const long double r=1)
{
	check_init();
	std::uniform_real_distribution<long double> urd(l,r);
	return urd(engine);
}

}


namespace JCfile{

const int STDIN=0,STDOUT=1;
const int stdin_backup=dup(STDIN);
const int stdout_backup=dup(STDOUT);

void ret_con()
{
	freopen("CON","r",stdin); dup2(stdin_backup,STDIN);
	freopen("CON","w",stdout); dup2(stdout_backup,STDOUT);
	return;
}

std::string dataname="unnamed";
std::string in_exs=".in",out_exs=".out";
const std::string default_cmd="-O2 -std=c++14 -static";
const std::string default_cpl="g++";

void set_dataname(const std::string &s)
{
	dataname=s;
	return;
}

std::string default_ctr(const int id)
{
	return dataname+std::to_string(id);
}

void gen_infile(const unsigned int L,const unsigned int R, void (*generator)(int), std::string (*counter)(int)=default_ctr)
{
	ret_con();
	JCtool::print_info("Generating input files\n");
	std::string dir="data-"+dataname;
	system(((std::string)"mkdir "+dir).c_str());
	JCtool::print_info("Make directory "+dir);
	for(unsigned int I=L;I<=R;I++)
	{
		std::string filename=dir+"\\"+counter(I);
		freopen((filename+in_exs).c_str(),"w",stdout);
		generator(I);
		ret_con();
		printf("\rGenerating... %d/%d",I-L+1,R-L+1);
	}
	putchar('\n');
	JCtool::print_success("Generated input files successfully.");
	return;
}

void compile(const std::string code_name, const std::string command=default_cmd)
{
	system(("g++ " + command+" " + code_name+".cpp " + "-o "+code_name).c_str());
	return;
}

void gen_outfile(const unsigned int L,const unsigned int R, const std::string program_name, std::string (*counter)(int)=default_ctr)
{
	ret_con();
	JCtool::print_info("Generating output files.");
	if(access((program_name+".exe").c_str(),F_OK))
	{
		JCtool::print_error(program_name+".exe"+" does not exist");
		return;
	}
	if(access((program_name+".exe").c_str(),X_OK))
	{
		JCtool::print_error("Unable to execute "+program_name+".exe");
		return;
	}
	bool end_line=false;
	for(unsigned int I=L;I<=R;I++)
	{
		std::string filename="data-"+dataname+"\\"+counter(I);
		if(access((filename+in_exs).c_str(),F_OK))
		{
			if(!end_line){putchar('\n');end_line=true;}
			JCtool::print_warning(filename+in_exs+" does not exist");
			continue;
		}
		if(access((filename+in_exs).c_str(),R_OK))
		{
			if(!end_line){putchar('\n');end_line=true;}
			JCtool::print_warning("Unable to read "+filename+in_exs);
			continue;
		}
		std::string command=program_name+".exe" + " < " + filename+in_exs + " > " + filename+out_exs;
		std::system(command.c_str());
		printf("\rGenerating... %d/%d",I-L+1,R-L+1);
		end_line=false;
	}
	putchar('\n');
	JCtool::print_success("Generated output files successfully.");
	return;
}

}

namespace JCmath{

const int INF=0x7fffffff;
const unsigned int INF_u=0xfffffff;
const long long INF_ll=0x7fffffffffffffff;
const unsigned long long INF_ull=0xffffffffffffffff;

}

#endif
