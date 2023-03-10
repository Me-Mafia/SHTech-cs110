#include <iostream>

using namespace std;

int main() {
	freopen("3.in", "r", stdin);
	int num; cin >> num;
	int character; int cnt;
	for (int j = 0; j < num; j++)
	{
		cin >> hex >> character;
		cnt = 0;
		for (int i = 0x080000; i != 0x00000; i >>= 1)
		{
			cnt++;
		  if (character & i)
		  {
			  cout << "■";
		  }
		  else
			  cout << " ";
		  if (cnt == 4)
		  {
			  cout << endl;
			  cnt = 0;
		  }
	  }
	  cout << endl;
	}
	return 0;
}

