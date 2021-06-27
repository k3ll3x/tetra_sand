#pragma once

#include "ifile.h"
#include "vec2.h"
#include <vector>
#include <string>
#include <regex>

//template<typename T>
class load_vec {
private:
    ifile vecf;
    //std::vector<T> v;
    std::string contents;
    std::regex regex;
public:
    load_vec(){
        regex = std::regex("[-]?\\d+.\\d+");
    }
    
    // template <typename T>
    // std::vector<T> (const std::string& filename){
    std::vector<cgmath::vec2> load_vec2(const std::string& filename){
        std::vector<cgmath::vec2> ps;
        vecf.read(filename);
        contents = vecf.get_contents();
        if(contents.size() > 1){
            auto begin = std::sregex_iterator(contents.begin(), contents.end(), regex);
            auto end = std::sregex_iterator();
            // int sz = std::distance(begin, end);
            for(std::sregex_iterator i = begin; i != end; i++){
                std::string xs = (*i).str();
                std::string ys = (*++i).str();
                cgmath::vec2 p(stof(xs), stof(ys));
                ps.push_back(p);
            }
        }
        return ps;
    }
};