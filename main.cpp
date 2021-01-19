#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <sstream>
#include <vector>
#include <iterator>
#include<dirent.h>
using namespace std;
#define EMPTY_STRING ""
using std::cout;
using std::fstream;
using std::endl;
using std::string;
unordered_map<char, string> huffmanCode;
map<char, int> freq;
unordered_map<char,string> codeMap;
#define INTERNAL_NODE_CHARACTER char(128)
#define PSEUDO_EOF char(129)
vector<string> files;






std::ifstream::pos_type filesize(string filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}




//  Tree
struct Node
{
    char ch;
    int freq;
    Node *left, *right;
    Node()
    {
        left=right=NULL;
    }
};

Node* root;


Node* getNode(char ch, int freq, Node* left, Node* right)
{
    Node* node = new Node();

    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;

    return node;
}


struct comp
{
    bool operator()(Node* l, Node* r)
    {
        // the highest priority item has the lowest frequency
        return l->freq > r->freq;
    }
};


bool isLeaf(Node* root)
{
    return root->left == nullptr && root->right == nullptr;
}

//////COMPRESSION
void encode(Node* root, string str, unordered_map<char, string> &huffmanCode)
{
    if (root == nullptr)
    {
        return;
    }

    if (isLeaf(root))
    {


        huffmanCode[root->ch] = (str != EMPTY_STRING) ? str : "1";
    }

    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}


string decimal_to_binary(int in)
{
    string temp = "";
    string result = "";
    while (in)
    {
        temp += ('0' + in % 2);
        in /= 2;
    }
    result.append(8 - temp.size(), '0');
    for (int i = temp.size() - 1; i >= 0; i--)
    {
        result += temp[i];
    }
    return result;
}


void binaryToByte(string file,ofstream &outputStream)
{
    unsigned long remainder = (file.size() - 1) % 8;
    for (int i = 0; i < 8 - remainder; ++i)
        file += '0';
    stringstream stringStream(file);

    while (stringStream.good())
    {
        bitset<8> bits;
        stringStream >> bits;
        char c = char(bits.to_ulong());
        outputStream << c;
    }
}


string readCMPFILE(string filename)
{
    fstream newfile;
    string tp;
    string temp;
    newfile.open(filename,ios::in);
    if (newfile.is_open())
    {
        while(getline(newfile, temp))
        {
            temp.append("|");
            tp.append(temp);
        }
        newfile.close();
    }
    else
    {
        cout<<'\n'<<"error in file opening"<<'\n';
        exit(1);
    }
    return tp;
}


void fileCompression(string text, string filename,int type)
{
    encode(root, EMPTY_STRING, huffmanCode);
    string str;
    for (char ch: text)
    {
        str += huffmanCode[ch];
    }
    cout<<"\n\t\t\t\t\tCharacter Table with Huffman codes \n";
    cout<<"\t======================================================================================================\n";
    cout<<"\n\tCharacter\t\t  ASCII\t\t\t   BinaryCode\t\t\tHuffman Code\n";
    cout<<"\t======================================================================================================\n";
    ofstream MyFile;
    string name ="cmp";
    name.append(filename);
    filename=name;
    if (type==1)
        MyFile.open (filename, std::ofstream::out);
    else if(type==0)
        MyFile.open (filename, std::ofstream::app);
    if (MyFile.is_open())
    {
        for (auto pair: huffmanCode)
        {
            MyFile << pair.first << "¥" << pair.second <<"§";
            cout <<"\t     "<< pair.first << "\t\t\t   " << (int)pair.first<< "\t\t\t   " << decimal_to_binary((int)pair.first) << "\t\t\t   " << pair.second <<"\n";
        }
        MyFile<<'Ø';
        MyFile<<'\n';
        for (auto pair: freq)
        {
            MyFile << pair.first << "¥" << pair.second <<"§";
        }
        MyFile<<'Ø';
        MyFile<<'\n';
        binaryToByte(str,MyFile);
    }
    else
    {
        cout<<'\n'<<"error in file opening"<<'\n';
        exit(1);
    }
    MyFile.close();
}

void buildHuffmanTree(string text)
{
    if (text == EMPTY_STRING)
    {
        return;
    }

    for (char ch: text)
    {
        freq[ch]++;
    }

    priority_queue<Node*, vector<Node*>, comp> pq;
    for (auto pair: freq)
    {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    while (pq.size() != 1)
    {
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();
        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }
    root = pq.top();
}

//////DECOMPRESSION
void decode(Node* root, string str,string filename)
{
    string ans = "";
    Node* curr = root;
    for (int i=0; i<str.size(); i++)
    {
        if (str[i] == '0')
            curr = curr->left;
        else
            curr = curr->right;

        if (curr->left==NULL and curr->right==NULL)
        {
            ans += curr->ch;
            curr = root;
        }
    }
    ans = ans+'\0';

    ofstream MyFile;
    string name ="De";
    name.append(filename);
    filename=name;
    std::ofstream clearFile;
    clearFile.open(filename, std::ofstream::out | std::ofstream::trunc);
    clearFile.close();
    MyFile.open (filename, std::ofstream::app );
    if(MyFile.is_open())
    {
        for(int j=0; j<ans.size(); j++)
        {
            if (ans[j] =='|')
                MyFile << '\n';
            else
                MyFile << ans[j];
        }
    }
    else
    {
        cout<<'\n'<<"error in file opening"<<'\n';
        exit(1);
    }
    MyFile.close();
    return;

}

string readDECMPfile(string filename)
{
    ifstream in_file;
    string line;
    char aCode;
    int bCode;
    string temp;
    string codeString;
    char character;
    int i;
    int flag;
    in_file.open(filename,  ios::in | std::ios::binary);
    if(in_file.is_open()==0)
    {
        cout<<'\n'<<"error in file opening"<<'\n';
        exit(1);
    }
    getline(in_file,temp);
    getline(in_file,line);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(in_file), {});
    std::string code(buffer.begin(),buffer.end());
    for(int j=0; j<code.size(); j++)
    {
        character=code[j];
        bitset<8> bits(character);
        codeString += bits.to_string();
    }
    for (i=0; line[i]!='Ø'; i++)
    {
        if(line[i]=='¥')
        {
            aCode=line[i-1];
            i++;
            bCode=0;
            flag=0;
            while(line[i]!='§')
            {
                if (flag==0)
                {
                    bCode=line[i]-48;
                    flag++;
                }
                else
                {
                    bCode*=10;
                    bCode+=(line[i]-48);
                    flag++;
                }
                i++;
            }
            freq.insert({aCode,bCode});
        }
    }
    return codeString;
}


void reBuildHuffmanTree()
{
    priority_queue<Node*, vector<Node*>, comp> pq;

    for (auto pair: freq)
    {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    while (pq.size() != 1)
    {
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();
        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }
    root = pq.top();
}



void fileDecompression(string line, string filename)
{
    if (isLeaf(root))
    {
        // Special case: For input like a, aa, aaa, etc.
        while (root->freq--)
        {
            cout << root->ch;
        }
    }
    else
    {
        decode(root, line,filename);
    }
}

//FOLDER COMPRESSION
void listFile()
{
    DIR *pDIR;
    struct dirent *entry;
    if( pDIR=opendir("./input") )
    {
        while(entry = readdir(pDIR))
        {
            if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 )
            {
                files.push_back(entry->d_name);
            }
        }
        closedir(pDIR);
    }
}

// Huffman coding algorithm
int main()
{
    cout<<"\n\t====================================";
    cout<<"\n\tText File Compressor & Decompressor\n";
    cout<<"\t====================================";
    while(1)
    {
        string code;
        int choose;
        printf("\n\nChoose:\n 1) file compression\n 2) file decompression\n 3) folder compression\n");
        printf("- Press any key to exit\n");
        cin>>choose;
        string text;
        switch(choose)
        {
        case 1 :
        {
            string filename;
            printf("Enter compression file name:");
            cin>>filename;
            string name ="cmp";
            name.append(filename);
            cout<<"\nProcessing.... Please wait !!!\n";
            clock_t tStart = clock();
            text=readCMPFILE(filename);
            buildHuffmanTree(text);
            fileCompression(text,filename,1);
            cout <<"Time taken: "<<(1.0*(clock() - tStart)/CLOCKS_PER_SEC)<<"sec"<<endl;
            cout << "Input File Size : "<<filesize(filename)<<" bytes."<<endl;
            cout<< "Compressed File Size : "<<filesize(name)<<" bytes."<<endl;
            cout<< "Compression Ratio : "<<(1.0*filesize(name)/filesize(filename))<<endl;
            cout<<"\nCompression Successful";
            break;
        }
        case 2:
        {
            string filename;
            printf("Enter decompression file name:");
            cin>>filename;
            string name ="De";
            name.append(filename);
            clock_t tStart = clock();
            code=readDECMPfile(filename);
            reBuildHuffmanTree();
            fileDecompression(code,filename);
            cout <<"Time taken: "<<(1.0*(clock() - tStart)/CLOCKS_PER_SEC)<<"sec"<<endl;
            cout << "Input File (Compressed) Size : "<<filesize(filename)<<" bytes."<<endl;
            cout<< "DeCompressed File Size : "<<filesize(name)<<" bytes."<<endl;
            cout<<"\nDecompression Successful";
            break;
        }
        case 3:
        {
            listFile();
            string foldername;
            string temp;
            printf("Enter compression folder name:");
            cin>>foldername;
            string filename = foldername;
            filename.append("/");
            int sum=0;
            clock_t tStart ;
            string name="cmp";
            foldername.append(".txt");
            name.append(foldername);
            string folderText;
            cout<<"\nProcessing.... Please wait !!!\n";
            tStart = clock();
            for (int i=0; i<files.size(); i++)
            {
                cout<<files[i];
                temp=filename;
                temp.append(files[i]);
                text=readCMPFILE(temp);
                folderText.append(text);
                sum+=filesize(temp);
            }
                buildHuffmanTree(folderText);
                fileCompression(folderText,foldername,0);
                cout <<"Time taken: "<<(1.0*(clock() - tStart)/CLOCKS_PER_SEC)<<"sec"<<endl;
                cout << "Input File Size : "<<sum<<" bytes."<<endl;
                cout<< "Compressed File Size : "<<filesize(name)<<" bytes."<<endl;
                cout<< "Compression Ratio : "<<(1.0*filesize(name)/sum)<<endl;
                cout<<"\nCompression Successful";
            break;
        }
        default :
        {
            exit(1);
        }
        }
    }
    return 0;
}
