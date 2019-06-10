#include <stdio.h>
//cout
#include <iostream>
//read file
#include <fstream>
//string
#include <string>
//read file name
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <dirent.h>
//vextor
//#include <vector.h>

namespace archive_creator {
    class creator {
        public :
            void create_archive();
            
        private :
            struct config_tree {
                std::string name;
                config_tree *next= nullptr;
                config_tree *deeper= nullptr;
            };
            
            struct archive_contents {
                std::string url;
                std::string description;
                std::string date;
                archive_contents *next = nullptr;
            };
            
            struct archive_tree {
                std::string name;
                archive_contents *contents = nullptr;
                archive_tree *next = nullptr;
                archive_tree *deeper = nullptr;
            };
            
            struct date_list {
                std::string url;
                std::string description;
                std::string date;
                int date_num;
                date_list *pre = nullptr;
                date_list *next = nullptr;
            };
            //read archive.config
            void read_config(creator::config_tree *);
            //create archive tree from config tree
            void create_archive_tree(creator::archive_tree *, creator::config_tree *, creator::config_tree *);
            ////get contents file name & store data
            //void read_files(creator::archive_tree *, creator::date_list *);
            //get file names of files in path
            void get_files(std::string, std::vector<std::string> &);
            //store file data to archive tree
            void store_file_data(creator::archive_tree *, creator::date_list *, std::vector<std::string> &, std::string);
            //get config data of file
            void get_data_of_file(std::string , std::string &);
            //store data to archive tree
            void store_data_to_tree(creator::archive_tree *, creator::date_list *,std::string , std::string &);
            //content  url date description
            void store_content(creator::archive_contents *, std::string, std::string, std::string);
            //list date(string) date(int) url description
            void store_date(creator::date_list *, std::string, std::string, int, std::string);
            bool delete_null_tree(creator::archive_tree *);
            
            
            
            void sort_data();
            //write archive
            void write_archive(creator::archive_tree *, int);
            
            void create_index();
            void create_hubs();
            void crate_contents();
            
            //print data
            void print_tree(creator::config_tree *, int);
            void print_tree(creator::archive_tree *, int);
            void print_list(creator::date_list *list);
            void print_contents(creator::archive_contents *, int);
            //delete data
            void delete_tree(creator::config_tree **);
            void delete_tree(creator::archive_tree **);
            void delete_tree(creator::archive_contents **);
            void delete_list(creator::date_list **);
        };
}
