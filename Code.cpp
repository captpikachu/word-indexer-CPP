#include <bits/stdc++.h>
#include <chrono>

using namespace std;

const int KBs = 1024;

class base {
public:
    int BUFFER_SIZE;
    virtual void tokenize(string & lastword) = 0; // This displays polymorphism 
    virtual ~base() {};
};

class versionIndexing : public base { 
public:
    map<string,int> word_index; 
    string version;
    string path;
    
    versionIndexing (string & version, string & path) { 
        this->version = version;
        this->path = path;
    }
// wordCount function displays overloading here , use void to directly print message in case of single version
    int wordCount(string & word) {
        auto it = word_index.find(word);
        return (it != word_index.end()) ? it->second : 0;
    }

    void wordCount(string & word, bool verbose) {
        int count = wordCount(word);
        if(verbose) {
            cout << "Version: " << version << " | Word: '" << word << "' | Count: " << count << endl;
        }
    }
    
};

class buffer : public versionIndexing {
public:
    buffer(string &v, string &p) : versionIndexing(v, p) {}

    void readBuffer(string & path, map<string,int> & word_index, int BUFFER_SIZE) {
        fstream file(path, ios::binary | ios::in);
        if(!file.is_open()) throw runtime_error("Failed to open file: " + path);

        string lastword;
        vector<char> internal_buf(BUFFER_SIZE);   
        while(file.read(internal_buf.data(), BUFFER_SIZE) || file.gcount() > 0) {
            for(int i = 0; i < file.gcount(); i++) {
                char c = internal_buf[i];
                if(!isalnum((unsigned char)c)) {
                    tokenize(lastword);
                    lastword.clear();
                }
                else {
                    lastword += tolower((unsigned char)c);
                }
            }
            
        }
        tokenize(lastword);
    }
};

class tokenization : public buffer {
public:
    tokenization(string &v, string &p) : buffer(v, p) {}
    
    // Polymorphism 
    void tokenize(string & lastword) override {
        if(!lastword.empty()) {
            word_index[lastword]++;
        }
    }
};

template <typename T>
class queryProcessing {
public:
    virtual void execute() = 0;
    virtual ~queryProcessing() {}
};

template <typename T>
class wordQuery : public queryProcessing<T> {
    T &v1;
    string target;
public:
    wordQuery(T &v, string &w) : v1(v), target(w) {}
    void execute() override { 
        v1.wordCount(target, true); 
    }
};

template <typename T>
class diffQuery : public queryProcessing<T> {
    T &v1; T &v2;
    string target;
public:
    diffQuery(T &a, T &b, string &w) : v1(a), v2(b), target(w) {}
    void execute() override {
        cout << "The difference between occurrence of '" << target << "' in both versions is " << abs(v1.wordCount(target) - v2.wordCount(target)) << endl;
    }
};

template <typename T>
class topKQuery : public queryProcessing<T> {
    T &v1;
    int topK;
public:
    topKQuery(T &v, int k) : v1(v), topK(k) {}
    void execute() override {
        vector<pair<string, int>> vec(v1.word_index.begin(), v1.word_index.end());
        int uniqueWords = vec.size();
        sort(vec.begin(), vec.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first < b.first;
        });

        cout << "Top " << min(topK, uniqueWords) << " frequent words in " << v1.version << ":" << endl;
        for(int i = 0; i < topK && i < uniqueWords; i++) {
            cout << i+1 << ". " << vec[i].first << " (" << vec[i].second << ")" << endl;
        }
        if(uniqueWords < topK)
            cout << "Note: Only " << uniqueWords << " unique words exist in version " << v1.version << endl;
    }
};

int main(int argc, char* argv[]) {
    string f1, f2, v1, v2, word, query;
    int topK = 10;
    int buffSize = 512 * KBs;

    try {
        if (argc < 2) throw runtime_error("No arguments provided. Use --query <word|diff|top>");

        for(int i = 1; i < argc; i++) {
            string arg = argv[i];
            if(arg == "--file" || arg == "--file1") f1 = argv[++i];
            else if(arg == "--file2") f2 = argv[++i];
            else if(arg == "--version" || arg == "--version1") v1 = argv[++i];
            else if(arg == "--version2") v2 = argv[++i];
            else if(arg == "--buffer") buffSize = stoi(argv[++i]) * KBs; 
            else if(arg == "--query") query = argv[++i];
            else if(arg == "--word") word = argv[++i];
            else if(arg == "--top") topK = stoi(argv[++i]);
        }

        if(buffSize < 256*KBs || buffSize > 1024*KBs)
            throw runtime_error("Buffer size must be between 256KB and 1024KB");

        for(auto &it : word) it = tolower((unsigned char)it);

        auto start_time = chrono::high_resolution_clock::now();
        unique_ptr<queryProcessing<tokenization>> qp;

        if(query == "word") {
            tokenization* t1 = new tokenization(v1, f1);
            t1->BUFFER_SIZE = buffSize;
            t1->readBuffer(f1, t1->word_index, t1->BUFFER_SIZE);
            qp = make_unique<wordQuery<tokenization>>(*t1, word);
        }
        else if(query == "diff") {
            tokenization* t1 = new tokenization(v1, f1);
            tokenization* t2 = new tokenization(v2, f2);
            t1->BUFFER_SIZE = t2->BUFFER_SIZE = buffSize;
            t1->readBuffer(f1, t1->word_index, t1->BUFFER_SIZE);
            t2->readBuffer(f2, t2->word_index, t2->BUFFER_SIZE);
            qp = make_unique<diffQuery<tokenization>>(*t1, *t2, word);
        }
        else if(query == "top") {
            tokenization* t1 = new tokenization(v1, f1);
            t1->BUFFER_SIZE = buffSize;
            t1->readBuffer(f1, t1->word_index, t1->BUFFER_SIZE);
            qp = make_unique<topKQuery<tokenization>>(*t1, topK);
        } else {
            throw runtime_error("Invalid query type. Use word, diff, or top.");
        }

        if (qp) qp->execute();

        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end_time - start_time;
        
        cout << "\nAllocated buffer size: " << buffSize / KBs << " KB" << endl;
        cout << "Total execution time: " << fixed << setprecision(6) << duration.count() << " seconds" << endl;
    }
    catch (const exception& e) {
        cerr << "RUNTIME ERROR: " << e.what() << endl;
        return 1;
    }
    return 0;
}   