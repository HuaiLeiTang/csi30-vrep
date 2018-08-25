#ifndef CPPREMOTEAPI_PATH_H
#define CPPREMOTEAPI_PATH_H

#import <vector>

void set_commands(const std::vector<int> &);

int get_next_command();

std::vector<int> depth_first_search();

#endif //CPPREMOTEAPI_PATH_H
