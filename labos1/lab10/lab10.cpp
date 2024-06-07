#define MAX_FDS 32

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <vector>

using namespace std;

//VFS objects
struct inode {
    dev_t st_dev;
    int i_ino; //inode file
    mode_t i_mode; // mode
    uid_t i_uid; //gid
    gid_t i_gid; //uid
    loff_t size; //size of file
    dev_t r_dev;
    unsigned int i_nlink; /* number of hard links */
    char *name;
}; 

struct file {
    int fd;
    char *pathname;       
};

//tables
vector<inode> inode_table;
vector<file> open_file_table;

struct fs_table {
    unsigned int max_fds = 32;
    struct file *fd[MAX_FDS];
};

//inode operations
void add_to_inode_table(char *pathname)
{
    struct inode new_inode;
    struct stat64 stat;
    stat64(pathname, &stat);
    new_inode.st_dev =  stat.st_dev;
    new_inode.i_ino =  stat.st_ino;
    new_inode.i_mode = stat.st_mode;
    new_inode.i_nlink = stat.st_nlink;
    new_inode.i_uid = stat.st_uid;
    new_inode.size = stat.st_size;
    new_inode.name = pathname;
    inode_table.push_back(new_inode);
}

void print_inodes(){
    cout<<"\n\n";
    cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
    cout<<"++++++++++++++++++++++++++++++++++++Inodes block+++++++++++++++++++++"<<endl;
    cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
    cout<<"st_dev\t|i_ino\t|i_mode\t|i_uid\t|i_gid\t|size\t|i_nlink"<<"|"<<"name"<<endl;
    for(int i = 0;i<inode_table.size(); i++){
        cout<<inode_table[i].st_dev<<"\t|"<<inode_table[i].i_ino<<"\t|"<<inode_table[i].i_mode<<"\t|"<<inode_table[i].i_uid<<"\t|"<<inode_table[i].i_gid
        <<"\t|"<<inode_table[i].size<<"\t|"<<inode_table[i].i_nlink<<"\t|"<<inode_table[i].name<<endl;
    }
}
//open fd operation

void add_file(int fd, char *pathname) {
    file new_file = {fd, pathname};
    open_file_table.push_back(new_file);
}

void close_file(int fd) {
    vector<file>::iterator it;
    it = open_file_table.begin();
    for(int i = 0;i<open_file_table.size();i++){
        if(open_file_table[i].fd==fd){
            open_file_table.erase(it+i);
        }
    }
}

void print_open_file(){
    cout<<"\n\n";
    cout<<"++++++++++++++++++++++++++++++++++"<<endl;
    cout<<"+++++++Open file table++++++++++++"<<endl;
    cout<<"++++++++++++++++++++++++++++++++++"<<endl;
    cout<<"fd\t|pathname"<<endl;
    for(int i = 0; i<open_file_table.size(); i++){
        cout<<open_file_table[i].fd<<"\t|"<<open_file_table[i].pathname<<endl;
    } 
}

void print_fd(){
    cout<<"\n\n";
    cout<<"++++++++++++++++++++++++++++++++++"<<endl;
    cout<<"+++++++++++++Fd table+++++++++++++"<<endl;
    cout<<"++++++++++++++++++++++++++++++++++"<<endl;
    system("ls -l /proc/self/fd");
}

void init(){
    cout<<"Starting process!"<<endl;
    cout<<"Open standart files"<<endl;
    add_to_inode_table("/dev/pts/0");
    add_file(0,"stdin");
    add_file(1,"stdout");
    add_file(2,"stderr");
}

void print_state()
{
    print_inodes();
    print_open_file();
    print_fd();
}

int main()
{
    int fd1;
    char *filep = "/home/kyrylshabanv/code/labs/labos1/lab10/example";
    char buff[30];
    cout<<"----------------------------------------------------------------------------------------"<<endl;
    //init
    //implicit opening input stream, output stream, etc.
    init();
    print_state();
    cout<<"----------------------------------------------------------------------------------------"<<endl;
    cout<<"Read input stream "<<endl;
    //read 5 bytes from 
    read(0,buff,5);
    cout<<"readed data:"<<buff<<endl;
    print_state();
    /*
        OPEN FILE
    */
    cout<<"----------------------------------------------------------------------------------------"<<endl;
    cout<<"\n\nOpen user file"<<endl;

    //creating and opening file

    fd1 = open(filep,O_RDONLY|O_WRONLY);
    add_to_inode_table(filep);
    add_file(fd1,filep);
    print_state();
    
    cout<<"----------------------------------------------------------------------------------------"<<endl;
    cout<<"Close file input"<<endl;
    close(0);
    close_file(0);
    print_state();
    cout<<"----------------------------------------------------------------------------------------"<<endl;
    cout<<"Duplicate user file "<<endl;
    int fd2 = dup(fd1);
    add_file(fd2,filep);
    print_state();
    cout<<"----------------------------------------------------------------------------------------"<<endl;
    cout<<"Close first user file "<<endl;
    close(fd1);
    close_file(fd1);
    print_state();
    cout<<"----------------------------------------------------------------------------------------"<<endl;
    cout<<"Read first 10 bytes from file "<<endl;
    char buff2[10];
    if((read(0,buff2,10))==-1){
        perror("read");
    }
    cout<<buff2<<endl;
    print_state();
    
}