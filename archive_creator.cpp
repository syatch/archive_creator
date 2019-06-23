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
    std::string contents_path = "./archive_contents/";
    //read archive.config and store data
    read_config(config_tree);
    //create archive_tree
    create_archive_tree(archive_tree, config_tree, config_tree);
    //read&store files
    std::vector<std::string> file_names;
    file_names.clear();
    //get name of files in contents_path
    get_files(file_names, contents_path);
    std::string save_path = "./contents/";
    //store contents data to tree
    store_file_data(archive_tree, date_list, file_names, contents_path, save_path);
    //delete tree that not used to store data
    delete_null_tree(archive_tree);
    //set path of contents
    save_path = "./build_page/contents/";
    //creates_content files
    create_contents(file_names, contents_path, save_path);
    //set path of pages
    std::string page_path = "./pages/";
    file_names.clear();
    //get name of files in page_path
    get_files(file_names, page_path);
    //create_archive_
    create_archive_text(archive_tree, file_names, page_path);
    /*
    //print_tree(config_tree, 0);
    if (config_tree != nullptr) {
        print_tree(config_tree, 0);
        delete_tree(&config_tree);
    }
    if (archive_tree != nullptr) {
        print_tree(archive_tree, 0);
        delete_tree(&archive_tree);
    }
    if (date_list != nullptr) {
        print_list(date_list);
        delete_list(&date_list);
    }
    */
}

//read archive.config
void creator::read_config(config_tree* tree)
{
    std::ifstream config("./config/archive.config");
    if (config.fail()) {
        std::cerr << "Failed to open archive.config" << std::endl;
        return;
    }

    bool first = true;
    std::string str;
    std::string get_word;
    config_tree *now_tree;
    config_tree *now_place;
    config_tree *head_place;
    now_tree = tree;
    while (getline(config, str)) {
        int size = str.size();
        int first_count = 0;
        int word_count = 0;
        
        //erase space
        for (int k = 0; k < size + 1; k++) {
            if (str[k] == ' ')
                str.erase(str.begin() + k--);
        }
        if (str[0] == '#') {
            first_count++;
            while (str[word_count] != ',' && word_count < size)
                word_count++;
            //get title
            get_word =  str.substr(first_count ,word_count - first_count);
            if (first) {
                tree->name = get_word;
                now_place = tree;
                first = false;
            } else {
                config_tree *p = new creator::config_tree;
                p->name = get_word;
                now_tree->next = p;
                now_tree = now_tree->next;
                now_place = now_tree;
            }
        } else if (str[0] == '&') {
        } else if (str[0] == '%') {
        } else if (str[0] == '!') {
        } else {
            //get priority
            bool first = true;
            while (word_count < size - 1) {
                while (str[word_count] != ',' && word_count != size)
                    word_count++;
                get_word = str.substr(first_count, word_count - first_count);
                word_count++;
                first_count = word_count;
                if (first) {
                    config_tree *p = new creator::config_tree;
                    p->name = get_word;
                    now_place->deeper = p;
                    now_place = now_place->deeper;
                    head_place = now_place;
                    first = false;
                } else {
                    config_tree *p = new creator::config_tree;
                    p->name = get_word;
                    now_place->next = p;
                    now_place = now_place->next;
                }
            }
            now_place = head_place;
        }
    }
    config.close();
}

//create archive tree from config
void creator::create_archive_tree(creator::archive_tree *tree, creator::config_tree *now_config, creator::config_tree *root_config)
{
    //if this config has name, store
    if (!now_config->name.empty())
        tree->name = now_config->name;

    //if next config exist, create
    if (now_config->next != nullptr) {
        archive_tree *p = new creator::archive_tree;
        tree->next = p;
        if (now_config->next->deeper != nullptr)
            create_archive_tree(p, now_config->next, now_config->next);
        else
            create_archive_tree(p, now_config->next, root_config);
    }

    //if deeper config exist, create
    if (root_config->deeper != nullptr) {
        archive_tree *p = new creator::archive_tree;
        tree->deeper = p;
        if (root_config->deeper->next == nullptr) {
            config_tree *p = new creator::config_tree;
            root_config->deeper->next = p;
        }
        create_archive_tree(p, root_config->deeper->next, root_config->deeper);
    }
}

//get contents file name
void creator::get_files(std::vector<std::string> &file_names, std::string path)
{
    int dirElements;
    std::string search_path;
    struct stat stat_buf;
    struct dirent **namelist=NULL;
    dirElements = scandir(path.c_str(), &namelist, NULL, NULL);
    if (dirElements == -1) {
        std::cout << "ERROR : No elements" <<  std::endl;
    } else {
        for (int i = 0; i < dirElements; i++) {
            if( (strcmp(namelist[i]->d_name , ".\0") != 0) && (strcmp(namelist[i]->d_name , "..\0") != 0) ) {
                search_path = path + std::string(namelist[i]->d_name);
                if(stat(search_path.c_str(), &stat_buf) == 0) {
                    if ((stat_buf.st_mode & S_IFMT) == S_IFDIR) {//if directory
                    }
                    else {
                        file_names.push_back(search_path);
                    }
                } else {
                    std::cout << "ERROR couldn't get file" << std::endl << std::endl;
                }
            }
        }
    }
    std::free(namelist);
}

//store data of contents
void creator::store_file_data(creator::archive_tree *tree, creator::date_list *date, std::vector<std::string> &files, std::string path, std::string save_path)
{
    for (int i = 0; i < files.size(); i++) {
        std::string data;
        get_data_of_file(files[i], data);
        store_data_to_tree(tree, date, files[i].substr(path.size(), files[i].size()-path.size()), data, save_path);
    }
}

//get data of file
void creator::get_data_of_file(std::string file, std::string &data)
{ 
    std::ifstream contents(file);
    if (contents.fail()) {
        std::cerr << "Failed to open " << file << std::endl;
        return;
    }

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
    contents.close();
}

void creator::store_data_to_tree(creator::archive_tree *tree, creator::date_list *date, std::string file, std::string &data, std::string save_path)
{
std::cout << "Start store data of " << file << std::endl;
    archive_tree *now_tree;
    now_tree = tree;
    archive_contents *now_contents;
    std::string get_word;
    std::string content_description;
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
        word_count++;
        first_count = word_count;
            
        if (get_word[0] == '&') {
            get_word =  get_word.substr(1, get_word.size() - 1);
            content_description = get_word;
        } else if (get_word[0] == '%') {
            get_word = get_word.substr(1, get_word.size() - 1);
            int date_first = 0;
            int date_count = 0;
            std::string get_date;
            std::string part_date;
            while (date_count <= get_word.size()) {
                while (get_word[date_count] != '/' && date_count != get_word.size())
                    date_count++;
                
                part_date = get_word.substr(date_first, date_count - date_first);
                if (part_date.size() == 1)
                    part_date = '0' + part_date;
                
                get_date = get_date + part_date;
                date_first = date_count;
                date_count++;
                date_first++;
            }
            int content_date = stoi(get_date);
            store_content(now_contents, save_path + file, get_word, content_description);
            store_date(date, save_path + file, get_word, content_date, content_description);
        } else if (get_word[0] == '!') {
        } else {
            while (true) {
                if (now_tree->name.empty())
                    now_tree->name = get_word;
                
                if (now_tree->name.compare(get_word) == 0) {
                    if (count_depth) {
                        archive_tree *p;
                        p = now_tree;
                        while (p->deeper != nullptr) {
                            p = p->deeper;
                            depth++;
                        }                    
                        count_depth = false;
                    } else if (depth == now_depth) {
                        if (now_tree->contents != nullptr) {
                            now_contents = now_tree->contents;
                        } else {
                            archive_contents *p = new creator::archive_contents;
                            now_tree->contents = p;
                            now_contents = p;
                        }
                        break;
                    } else {
                        if (now_tree->deeper != nullptr) {
                            now_tree = now_tree->deeper;
                        } else {
                            archive_tree *p = new creator::archive_tree;
                            p->name = get_word;
                            now_tree->deeper = p;
                            now_tree = now_tree->deeper;
                        }
                        now_depth++;
                    }
                } else if (now_tree->next != nullptr) {
                    now_tree = now_tree->next;
                } else {
                    archive_tree *p = new creator::archive_tree;
                    p->name = get_word;
                    now_tree->next = p;
                    now_tree = now_tree->next;
                }
            } 
        }    
    }
std::cout << "End store data of " << file << std::endl;
}

void creator::store_content(creator::archive_contents *now_contents, std::string url, std::string date, std::string content_description)
{
    if (now_contents->url.empty()) {
        now_contents->url = url;
        now_contents->date = date;
        now_contents->description = content_description;
    } else if (now_contents->next != nullptr) {
        store_content(now_contents->next, url, date, content_description);
    } else {
        archive_contents *p = new creator::archive_contents;
        p->url = url;
        p->date = date;
        p->description = content_description;
        now_contents->next = p;
    }
}

void creator::store_date(creator::date_list *now_date, std::string url, std::string date, int content_date, std::string content_description)
{
    while (true) {
        if (now_date->url.empty()) {
            now_date->url = url;
            now_date->description = content_description;
            now_date->date = date;
            now_date->date_num = content_date;
            break;
        } else if (content_date >= now_date->date_num) {
            date_list *p = new creator::date_list;
            p->url = now_date->url;
            p->description = now_date->description;
            p->date = now_date->date;
            p->date_num = now_date->date_num;
            
            now_date->url = url;
            now_date->description = content_description;
            now_date->date = date;
            now_date->date_num = content_date;
            
            p->next = now_date->next;
            now_date->next = p;
            break;
        }
                
        if (now_date -> next != nullptr) {
            now_date = now_date->next;
        } else {
            date_list *p = new creator::date_list;
            p->url = url;
            p->description = content_description;
            p->date = date;
            p->date_num = content_date;
            now_date->next = p;
            break;
        }
    }
}

bool creator::delete_null_tree(creator::archive_tree *tree)
{
    bool empty = false;
    //if next is empty, delete next
    if (tree->next != nullptr) {
        empty = delete_null_tree(tree->next);
        if (empty) {         
            if (tree->next->next != nullptr) {
                archive_tree **p;
                p = &(tree->next->next);
                delete tree->next;
                tree->next = *p;
            } else {
                delete tree->next;
                tree->next = nullptr;
            }
        }
    }
    //if deeper is empty, delete empty
    empty = false;
    if (tree->deeper != nullptr) {
        empty = delete_null_tree(tree->deeper);
        if (empty) {
            if (tree->deeper->next != nullptr) {
                archive_tree **p;
                p = &(tree->deeper->next);
                delete tree->deeper;
                tree->deeper = *p;
            } else {
                delete tree->deeper;
                tree->deeper = nullptr;
            }  
        }
    }
    //return if this tree is not used to store data
    if ((tree->deeper == nullptr) && (tree->contents == nullptr))
        return true;
    else
        return false;    
}

void creator::create_contents(std::vector<std::string> &files, std::string path, std::string save_path) {
    std::ifstream template_file("./archive_template/archive_contents_template.html");
    if (template_file.fail()) {
        std::cerr << "failed to open " << "./archive_template/archive_contents_template.html" << std::endl;
        return;
    }

    std::istreambuf_iterator<char> template_it(template_file);
    std::istreambuf_iterator<char> template_last;
    std::string template_str(template_it, template_last);
    template_file.close();
    
    for (int i = 0; i < files.size(); i++) {      
        if (files[i].substr(files[i].size() - 5, 5) == ".html") {
            std::ifstream contents_file(files[i]);
            if (contents_file.fail())
                std::cerr << "failed to open " << files[i] << std::endl;
            
            std::istreambuf_iterator<char> contents_it(contents_file);
            std::istreambuf_iterator<char> contents_last;
            std::string contents_str(contents_it, contents_last);
            contents_file.close();
            
            bool index = false;
            int place;
            for (int i = 0; i < template_str.size() - 16; i++) {
                if (template_str.substr(i, 16) == "archive_contents")
                    index = true;
                if ((index == true) && (template_str[i + 16] == '>')) {
                    place = i + 17;
                    break;
                }
            }
            std::string str = template_str.substr(0, place) + "\n" + contents_str + template_str.substr(place, str.size() - place);
            std::ofstream outfile(save_path + files[i].substr(path.size(), files[i].size()-path.size()));
            outfile<<str;
            outfile.close();
        }
    }
}

void creator::create_archive_text(creator::archive_tree *tree, std::vector<std::string> &files, std::string path)
{
    //create index_text
    std::string index_texts;
    index_texts = create_index_text(tree);
    //create menu index
    for (int i = 0; i < files.size(); i += 1) {
       // create_index(index_texts, files[i], path);
        if (files[i].substr(files[i].size() - 5, 5) == ".html") {
            std::ifstream file(files[i]);
            if (file.fail()) {
                std::cerr << "failed to open " << files[i] << std::endl;
                return;
            }
    
            std::istreambuf_iterator<char> it(file);
            std::istreambuf_iterator<char> last;
            std::string str(it, last);
            file.close();
            bool index = false;
            int place;
            for (int i = 0; i < str.size() - 12; i++) {
                if (str.substr(i, 12) == "archive_main")
                    index = true;
                if ((index == true) && (str[i + 12] == '>')) {
                    place = i + 13;
                    break;
                }
            }
        
            if (index) {
                str = str.substr(0, place) + index_texts + str.substr(place, str.size() - place);
                std::ofstream outfile("./build_page/" + files[i].substr(path.size(), files[i].size()-path.size()));
                outfile<<str;
                outfile.close();
            }
        }    
    }
    //create hub_text    
    create_hub(tree);
}

std::string creator::create_index_text(creator::archive_tree *tree)
{
    std::ifstream config("./config/archive_index_text.config");
    if (config.fail()) {
        std::cerr << "failed to open archive_index_text.config" << std::endl;
    }
    
    std::istreambuf_iterator<char> it(config);
    std::istreambuf_iterator<char> last;
    std::string str(it, last);
    std::vector<std::string> template_texts;
    template_texts.clear();

    int start = 0;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '$')
            start = i + 1;
        if (str[i] == '#')
            template_texts.push_back(str.substr(start, i - start));
    } 
    std::string result;
    result += template_texts[0];
    result += template_texts[1];
    archive_tree *now;
    now = tree;
    int num = 0;
    while (now != nullptr) {
        result += template_texts[2] + "hub" + std::to_string(num++) + ".html" + template_texts[3] + now->name + template_texts[4];
        now = now->next;
    }
    result += template_texts[5];
    return result;
}

void creator::create_hub(creator::archive_tree *tree)
{
    //get hub config
    std::ifstream config("./config/archive_hub_text.config");
    if (config.fail())
        std::cerr << "failed to open archive_index_text.config" << std::endl;

    std::istreambuf_iterator<char> it(config);
    std::istreambuf_iterator<char> last;
    std::string str_hub(it, last);
    std::vector<std::string> hub_texts;
    hub_texts.clear();

    int start = 0;
    for (int i = 0; i < str_hub.size(); i++) {
        if (str_hub[i] == '$')
            start = i + 1;
        if (str_hub[i] == '#')
            hub_texts.push_back(str_hub.substr(start, i - start));
    } 
    config.close();
    
    //get hub template
    std::ifstream template_file("./archive_template/archive_template.html");
    if (template_file.fail()) {
        std::cerr << "failed to open " << "./archive_template/archive_template.html" << std::endl;
        return;
    }

    std::istreambuf_iterator<char> template_it(template_file);
    std::istreambuf_iterator<char> template_last;
    std::string template_str(template_it, template_last);
    template_file.close();
            
    bool index = false;
    int place;
    for (int i = 0; i < template_str.size() - 11; i++) {
        if ((template_str.substr(i, 11) == "archive_hub") && (template_str[i+11] != '.'))
            index = true;
        if ((index == true) && (template_str[i + 11] == '>')) {
            place = i + 12;
            break;
        }
    }
    
    //create text and page
    archive_tree *now;
    now = tree;
    int num = 0;
    int hub_num = 0;
    while (now != nullptr) {
        //create index of hub
        std::string text;
        text += hub_texts[0];
        text += now->name;
        text += hub_texts[1];
        text += hub_texts[2];
        create_hub_index(now->deeper, hub_texts, text, &num);
        text += hub_texts[8];
        now = now->next; 
        num++;
        //create text of hub
        std::string str = template_str.substr(0, place) + "\n" + text + template_str.substr(place, str.size() - place);
        //save hub
        std::ofstream outfile("./build_page/hub" + std::to_string(hub_num++) + ".html");
        outfile<<str;
        outfile.close();
    }
}

void creator::create_hub_index(creator::archive_tree *tree, std::vector<std::string> &template_texts, std::string &text, int *num)
{
    int my_num = *num;
    text += template_texts[3];
    text += std::to_string(my_num);
    text += template_texts[4];
    text += std::to_string(my_num);
    text += template_texts[5];
    text += tree->name;
    text += template_texts[6];
    
    if (tree->contents != nullptr) {
        text += template_texts[9];
        text += tree->contents->url;
        text += template_texts[10];
        text += tree->contents->description;
        text += template_texts[11];
    }
    if (tree->deeper != nullptr) {
        text += template_texts[2];
        (*num)++;
        create_hub_index(tree->deeper, template_texts, text, num);
        text += template_texts[8];
    }
    text += template_texts[7];
    if (tree->next != nullptr) {
        (*num)++;
        create_hub_index(tree->next, template_texts, text, num);
    }
}

//print tree for debug
void creator::print_tree(creator::config_tree *tree, int indent)
{
    std::string print;
    for (int i = 0; i < indent; i++)
        print = " " + print;
    
    std::cout << print + tree->name << std::endl;
    if (tree->deeper != nullptr)
        print_tree(tree->deeper, indent + 1);
    if (tree->next != nullptr)
        print_tree(tree->next, indent);
}

void creator::print_tree(creator::archive_tree *tree, int indent)
{
    std::string print;
    for (int i = 0; i < indent; i++)
        print = " " + print;
   
    std::cout << print + tree->name << std::endl;
    if (tree->deeper != nullptr)
        print_tree(tree->deeper, indent + 1);
    if(tree->contents != nullptr)
        print_contents(tree->contents, indent + 1);
    if (tree->next != nullptr)
        print_tree(tree->next, indent);
}

void creator::print_list(creator::date_list *list)
{
    std::cout << list->date << std::endl;
    std::cout << list->description << std::endl;
    std::cout << list->url << std::endl;

    if (list->next != nullptr)
        print_list(list->next);
}

void creator::print_contents(creator::archive_contents *content, int indent)
{
    std::string print;
    for (int i = 0; i < indent; i++)
        print = " " + print;
    
    std::cout << print + ":" + content->description << std::endl;
    if (content->next != nullptr)
        print_contents(content->next, indent);
}

void creator::delete_tree(creator::config_tree **tree)
{
    if ((*tree)->next != nullptr)
        delete_tree(&(*tree)->next);
    if ((*tree)->deeper != nullptr)
        delete_tree(&(*tree)->deeper);
    delete *tree;
}

void creator::delete_tree(creator::archive_tree **tree)
{
    if ((*tree)->next != nullptr)
        delete_tree(&(*tree)->next);
    if ((*tree)->deeper != nullptr)
        delete_tree(&(*tree)->deeper);
    if ((*tree)->contents != nullptr)
        delete_tree(&(*tree)->contents);
    delete *tree;
}

void creator::delete_tree(creator::archive_contents **tree)
{
    if ((*tree)->next != nullptr)
        delete_tree(&(*tree)->next);
    delete *tree;

}

void creator::delete_list(creator::date_list **list)
{
    if ((*list)->next != nullptr)
        delete_list(&(*list)->next);
    delete *list;
}
