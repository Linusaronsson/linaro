#include <iostream>
#include <unordered_map>

struct Signature {
	public:
	std::string name;
	int num_args;
	Signature(std::string n, int a) : name(n), num_args(a) {}
	
	 bool operator==(const Signature &other) const { 
		return name == other.name && num_args == other.num_args;
	 }
};

struct KeyHasher
{
  std::size_t operator()(const Signature& k) const
  {
    return (std::hash<std::string>()(k.name)) ^ (std::hash<int>()(k.num_args) << 1);
  }
};

int index = 10;
std::unordered_map<Signature, int, KeyHasher> t;

void insert(Signature& s) {
	if (t.find(s) == t.end()) {
		t[s] = index++;		
		std::cout << "inserted with index: " << index-1 << std::endl;
		return;
	}
	std::cout << s.name << " already defined" << std::endl;
}

int lookup(Signature& s) {
	if(t.find(s) != t.end()) {
		return t[s];
	}
	return -1;
}

int main() {

	Signature f1("func1", 0);
	Signature f2("func1", 2);
	Signature f3("asdasdasd", 999);
	insert(f1);
	//insert(f2);
	std::cout << t[f2];
	std::cout << t[f3];
	return 0;
}