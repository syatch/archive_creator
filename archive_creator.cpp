#include "archive_creator.hpp"

using namespace archive_creator;

int main (void)
{
    creator archive;
    archive.create_archive();
}

void creator::create_archive()
{
    config_tree *config_tree = new creator::config_tree;
    archive_tree *archive_tree = new creator::archive_tree;
    date_list *date_list = new creator::date_list;
    //read archive.config and store data
    read_config(config_tree);
    //create archive_tree
    create_archive_tree(archive_tree, config_tree, config_tree);
    //read&store files
    read_files(archive_tree, date_list);
    write_archive(archive_tree);
    std::cout << "fin" << std::endl;
    
    print_tree(config_tree);
}

//read archive.config
void creator::read_config(config_tree* tree)
{
    std::cout << "read config" << std::endl;
    std::ifstream config("archive.config");
    if (config.fail()) {
        std::cerr << "Failed to open archive.config" << std::endl;
    }    
    bool first = true;
    std::string str;
    std::string get_word;
    config_tree *now_tree;
    config_tree *now_place;
    config_tree *head_place;
    now_tree = tree;
    while (getline(config, str)) {
        std::cout << "get : " << str << std::endl;
        int size = str.size();
        int first_count = 0;
        int word_count = 0;
        
        //erase space
        for (int k = 0; k < size + 1; k++) {
            if (str[k] == ' ')
                str.erase(str.begin() + k--);
            //if (str[k] == '\0')
            //    str.erase(str.begin() + k--);
        }
        if (str[0] == '#') {
            first_count++;
            while (str[word_count] != ',' && word_count != size)
                word_count++;
            //get title
            get_word =  str.substr(first_count ,word_count - first_count);
            if (first) {
                //get_word.push_back('\0');
                tree->name = get_word;
                now_place = tree;
                std::cout << "save title : " << tree->name << std::endl;
                first = false;
            }
            else {
                config_tree *p = new creator::config_tree;
                //get_word.push_back('\0');
                p->name = get_word;
                now_tree->next = p;
                now_tree = now_tree->next;
                now_place = now_tree;
                std::cout << "save title : " << now_tree->name << std::endl;
            }
        } else if (str[0] == '&') {
        } else if (str[0] == '%') {
        } else if (str[0] == '!') {
        } else {
            //get priority
            bool first = true;
            while (word_count <= size) {
                while (str[word_count] != ',' && word_count != size)
                    word_count++;
                get_word = str.substr(first_count ,word_count - first_count);
                std::cout << "get word : " << get_word << std::endl;
                word_count++;
                first_count = word_count;
                if (first) {
                    config_tree *p = new creator::config_tree;
                    //get_word.push_back('\0');
                    p->name = get_word;
                    now_place->deeper = p;
                    now_place = now_place->deeper;
                    head_place = now_place;
                    std::cout << "save deep : " << now_tree->name << std::endl;
                    std::cout << "- " << now_place->name << std::endl;
                    first = false;
                } else {
                    config_tree *p = new creator::config_tree;
                    //get_word.push_back('\0');
                    p->name = get_word;
                    now_place->next = p;
                    now_place = now_place->next;
                    std::cout << "save word : " << now_tree->name << std::endl;
                    std::cout << "- " << now_place->name << std::endl;
                }
            }
            now_place = head_place;
        }
    }
}

void creator::create_archive_tree(creator::archive_tree *tree, creator::config_tree *now_config, creator::config_tree *root_config)
{
    std::cout << "in create_archive_tree" << std::endl;
    //std::cout << now_config->name << std::endl;
    if (!now_config->name.empty()) {
        //std::cout << "not empty" << std::endl;
        tree->name = now_config->name;
        //std::cout << "save name  " << tree->name << std::endl;
    }

    std::cout << "next?" << std::endl;
    if (now_config->next != nullptr) {
        //std::cout << "go next" << std::endl;
        archive_tree *p = new creator::archive_tree;
        tree->next = p;
        if (now_config->next->deeper != nullptr)
            create_archive_tree(p, now_config->next, now_config->next);
        else
            create_archive_tree(p, now_config->next, root_config);
    }

    std::cout << "deeper?" << std::endl;
    if (root_config->deeper != nullptr) {
        //std::cout << "go deeper" << std::endl;
        archive_tree *p = new creator::archive_tree;
        tree->deeper = p;
        //std::cout << "add deeper" << std::endl;
        //wheen deeper->next not exist, segmentetion fault 
        if (root_config->deeper->next == nullptr) {
            config_tree *p = new creator::config_tree;
            root_config->deeper->next = p;
        }
        create_archive_tree(p, root_config->deeper->next, root_config->deeper);
    }
    //std::cout << "end" << std::endl;
    
}

//get contents file name & store data
void creator::read_files(creator::archive_tree *tree, creator::date_list *date)
{
    std::cout << "read file" << std::endl;
    std::vector<std::string> file_names;
    file_names.clear();
    std::string contents_path = "./archive_contents/";
    //get name of files in contents_path/
    get_files(contents_path, file_names);
    for(int i = 0; i < file_names.size(); i += 1)
        std::cout << file_names[i] << std::endl;
    //store contents data to tree
    store_contents_to_tree(tree, date, file_names, contents_path);
}

//get contents file name
void creator::get_files(std::string path, std::vector<std::string> &file_names)
{
    int dirElements;
    std::string search_path;
    struct stat stat_buf;
    struct dirent **namelist=NULL;
    dirElements = scandir(path.c_str(), &namelist, NULL, NULL);
    if(dirElements == -1) {
        std::cout << "ERROR : No elements" <<  std::endl;
    }
    else{
        for (int i = 0; i < dirElements; i++) {
            if( (strcmp(namelist[i]->d_name , ".\0") != 0) && (strcmp(namelist[i]->d_name , "..\0") != 0) ) {
                search_path = path + std::string(namelist[i]->d_name);
                if(stat(search_path.c_str(), &stat_buf) == 0) {
                    if ((stat_buf.st_mode & S_IFMT) == S_IFDIR) {//if directory
                    }
                    else {
                        file_names.push_back(search_path);
                    }
                }
                else{
                    std::cout << "ERROR couldn't get file" << std::endl << std::endl;
                }
            }
        }
    }
    std::free(namelist);
}

//store data of contents
void creator::store_contents_to_tree(creator::archive_tree *tree, creator::date_list *date, std::vector<std::string> &files, std::string path)
{
    std::cout << "store file" << std::endl;
    for (int i = 0; i < files.size(); i += 1) {
        std::string data;
        get_data_of_file(files[i], data);
        //std::cout << files[i] << std::endl;
        std::cout << "get data of contents : " << files[i].substr(path.size(), files[i].size()-path.size()) << std::endl;
        std::cout << data << std::endl; 
        store_data_to_tree(tree, date, files[i].substr(path.size(), files[i].size()-path.size()), data);
    }
}

void creator::get_data_of_file(std::string file, std::string &data)
{ 
    //std::cout << file << std::endl;
    std::ifstream contents(file);
    if (contents.fail())
        std::cerr << "Failed to open " << file << std::endl;

    //get content's data
    if (getline(contents, data)) {
        //get first and end of data
        int first_count = 0;
        int end_count = 0;
        int j = 0;
        //delete ' '
        for (int k = 0; k < data.size() + 1; k++) {
            if (data[k] == ' ')
                data.erase(data.begin() + k--);
            //if (data[k] == '\0')
            //    data.erase(data.begin() + k--);
        }
        while ((data[j] == '<') || (data[j] == '!') || (data[j] == '-')) {
            j++;
            first_count++;
        }
        while (data[j] != '-')
            j++;
        end_count = data.size() - j;
        //mask data
        data = data.substr(first_count, data.size() - (first_count + end_count));
    } else {
        std::cout << "ERROR couldn't get strings" << std::endl << std::endl;
    }     
}

void creator::store_data_to_tree(creator::archive_tree *tree, creator::date_list *date, std::string file, std::string &data)
{
    std::cout << "in store data to tree" << std::endl;
    std::cout << file << std::endl;
    archive_tree *now_tree;
    archive_contents *now_contents;
    date_list *now_date;
    now_tree = tree;
    std::string save_path = "./archive_contents/";
    std::string get_word;
    int size = data.size();
    int word_count = 0;
    int first_count = 0;
    int depth = 0;
    int now_depth = 0;
    bool count_depth = true;
    while (word_count <= size) {
        while (data[word_count] != ',' && word_count != size)
            word_count++;
        get_word = data.substr(first_count ,word_count - first_count);
        std::cout << "search & save word : " << get_word << std::endl;
        std::cout << get_word << std::endl;
        word_count++;
        first_count = word_count;
            
        if (get_word[0] == '&') {
            std::cout << "store contents" << std::endl;
            get_word =  get_word.substr(1, get_word.size() - 1);
            while (true) {
                if (now_contents->url.empty()) {
                    std::cout << "empty" << std::endl;
                    now_contents->url = save_path + file;
                    now_contents->description = get_word;
                    std::cout << now_contents->url << std::endl;
                    std::cout << now_contents->description << std::endl;
                    break;
                } else if (now_contents->next != nullptr) {
                    now_contents = now_contents->next;
                    std::cout << "go next" << std::endl;
                } else {
                    std::cout << "create new contents" << std::endl;
                    archive_contents *p = new creator::archive_contents;
                    now_contents->next = p;
                    p->url = save_path + file;
                    now_contents->description = get_word;
                    std::cout << now_contents->url << std::endl;
                    std::cout << now_contents->description << std::endl;
                    break;
                }
            }
        } else if (get_word[0] == '%') {
            get_word = get_word.substr(1, get_word.size() - 1);
            std::cout << get_word << std::endl;
            int date_first = 0;
            int date_count = 0;
            std::string get_date;
            std::string part_date;
            while (date_count <= get_word.size()) {
                while (get_word[date_count] != '/' && date_count != get_word.size()) {
                    date_count++;
                }
                part_date = get_word.substr(date_first, date_count - date_first);
                //std::cout << part_date.size() << std::endl;
                if (part_date.size() == 1)
                    //part_date.insert(part_date.begin() + 0, '0');
                    part_date = '0' + part_date;
                
                get_date = get_date + part_date;
                std::cout << get_date << std::endl;
                date_first = date_count;
                date_count++;
                date_first++;
            }
            int content_date = stoi(get_date);
            printf("date : %d\n",content_date);
        
        
        
        /*
            now_date = date;
            while (true) {
                if (now_date->url.empty()) {
                    std::cout << "date : empty" << std::endl;
                } else if (  date is newer than (>=)  now_date) {
                    date_list *p = new creator::date_list;
                    p->next = now_list->next;
                    now_list->next = p;
                    break;
                }
                
                if (now_date -> next != nullptr) {
                    now_date = now_date->next;
                } else {
                    date_list *p = new creator::date_list;
                    now_list->next = p;
                    break;
                }
            
            }
          */  
            
            
            /*
            now_date = date;
            while (true) {
                if (now_date)            
            
            }
            */
        } else if (get_word[0] == '!') {
        } else {
            std::cout << "in tree" << std::endl;
            std::cout << get_word << std::endl;
            while (true) {
                if (now_tree->name.empty()) {
                    std::cout << "found empty sector" << std::endl;
                    now_tree->name = get_word;
                }
                std::cout << "now name a" << now_tree->name << "a" << std::endl;
                std::cout << "get name a" << get_word << "a" << std::endl;
                std::cout << "result " << now_tree->name.compare(get_word) << std::endl;
                
                if (now_tree->name.compare(get_word) == 0) {//fail to compare
                    if (count_depth) {
                        archive_tree *p;
                        p = now_tree;
                        while (p->deeper != nullptr) {
                            p = p->deeper;
                            depth++;
                        }                    
                        std::cout << "depth : " << depth << std::endl;
                        now_depth++;
                        count_depth = false;
                    } else if (depth == now_depth) {
                        if (now_tree->contents != nullptr) {
                            now_contents = now_tree->contents;
                        } else {
                            std::cout << "go contents" << std::endl;
                            archive_contents *p = new creator::archive_contents;
                            now_tree->contents = p;
                            now_contents = p;
                        }
                    } else {
                        now_depth++;
                    }
                    std::cout << "found same name go deeper" << std::endl;
                    now_tree = now_tree->deeper;
                    break;
                } else if (now_tree->next != nullptr) {
                    std::cout << "go next" << std::endl;
                    now_tree = now_tree->next;
                } else {
                    std::cout << "create new next" << std::endl;
                    archive_tree *p = new creator::archive_tree;
                    p->name = get_word;
                    now_tree->next = p;
                    now_tree = now_tree->next;
                }
                std::cout << "next step" << std::endl;
            } 
        }    
    }
}

void creator::write_archive(creator::archive_tree *tree)
{
    //sort_data();
    std::cout << "write archive" << std::endl;
}

void creator::sort_data()
{
    std::cout << "sort data" << std::endl;
}

//read and  tree
void creator::print_tree(creator::config_tree * tree)
{
    std::cout << tree->name << std::endl;
    if (tree->deeper != nullptr) {
        std::cout << "go deeper : " << tree->deeper->name << std::endl;
        print_tree(tree->deeper);
        std::cout << "end : " << tree->deeper->name << std::endl;
    }
    if (tree->next != nullptr) {
        std::cout << "go next : " << tree->next->name << std::endl;
        print_tree(tree->next);
    }
}

void creator::print_config_tree(creator::config_tree * tree)
{

}
