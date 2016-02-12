#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <vector>
#include <regex>

using namespace std;

void RunCommand(vector<string> v){

	const int n = v.size();

	int parr[n - 1][2];

	for(int i = 0; i < n - 1; ++i){

		//Create pipe
		pipe(parr[i]);

		if(!fork()){

			if(i == 0){
				close(STDOUT_FILENO);

				dup2(parr[0][1], STDOUT_FILENO);

				close(parr[0][1]);
				close(parr[0][0]);

				execlp("sh", "sh", "-c", v[i].c_str(), NULL);
			}

			else{
				close(STDIN_FILENO);
				close(STDOUT_FILENO);

				dup2(parr[i - 1][0], STDIN_FILENO);
				dup2(parr[i][1], STDOUT_FILENO);

				for(int j = 0; j <= i; ++j){
					close(parr[j][0]);
					close(parr[j][1]);
				}

				execlp("sh", "sh", "-c", v[i].c_str(), NULL);

			}


			exit(0);
		}

		else{
			//wait(NULL);
		}

	}

	close(STDIN_FILENO);

	dup2(parr[n - 2][0], STDIN_FILENO);

	for(int j = 0; j < n - 1; ++j){
		close(parr[j][0]);
		close(parr[j][1]);
	}

	execlp("sh", "sh", "-c", v[n - 1].c_str(), NULL);
}


int main(int argc, char **argv) {

	string input("who | sort | uniq -c | sort -nk1");

	std::regex commandReg("([a-z]+)(\\s*(-[a-z0-9]+)*)*");

	std::regex_iterator<std::string::iterator> reg_it(input.begin(), input.end(), commandReg);
	std::regex_iterator<std::string::iterator> reg_it_end;

	vector<string> v;

	while(reg_it != reg_it_end){

		v.push_back(reg_it->str());

		++reg_it;
	}

	RunCommand(v);

	return 0;
}
