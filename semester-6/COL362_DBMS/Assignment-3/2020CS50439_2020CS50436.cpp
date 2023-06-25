#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <iterator>

int external_merge_sort_withstop(const char *input, const char *output, const long key_count, const int k = 2, const int num_merges = 0)
{

    int stringsize = 1024;
    int MAXSIZEMAX = 90e7 / stringsize;
    int MAXSIZE = MAXSIZEMAX / (k + 1);

    std::string text;
    std::stringstream buffer;
    std::vector<std::string> initial_vector;
    int n = 0;
    int length;
    int key_check = 0;
    std::ifstream start_file(input, std::ios::binary);
    std::ofstream filename;
    int maxsizeofstr = 0 ; 
    while (getline(start_file, text) && (key_check < key_count || key_count == 0))
    {
        maxsizeofstr = std::max(maxsizeofstr , (int)text.size());
        key_check += 1;
        initial_vector.push_back(text);
        if (initial_vector.size() == MAXSIZEMAX)
        {
            filename.open("temp.0." + std::to_string(n), std::ios::binary);
            sort(initial_vector.begin(), initial_vector.end());
            length = 0;
            while (length < MAXSIZEMAX)
            {
                buffer << initial_vector[length] << "\n";
                length += 1;
                if (length % MAXSIZE == 0)
                {
                    filename << buffer.str();
                    buffer.str(std::string());
                }
            }

            // std::move(initial_vector.begin(), initial_vector.end(), std::ostream_iterator<std::string>(buffer, "\n"));
            filename << buffer.str();

            buffer.str(std::string());
            filename.close();
            initial_vector = {};
            n += 1;
        }
    }
    
    if (initial_vector.size() != 0)
    {
        sort(initial_vector.begin(), initial_vector.end());

        filename.open("temp.0." + std::to_string(n));

        // std::move(initial_vector.begin(), initial_vector.end(), std::ostream_iterator<std::string>(buffer, "\n"));
        length = 0 ;
        while (length < initial_vector.size())
        {
            buffer << initial_vector[length] << "\n";
            length += 1;
            {
                filename << buffer.str();
                buffer.str(std::string());
            }
        }
        filename << buffer.str();

        buffer.str(std::string());
        filename.close();
        initial_vector = {};
        n += 1;
    }
    start_file.close();
    int merge_number = 0;
    int empty_index;
    int FilesToOpen;
    std::ifstream files[k];
    std::ofstream file_next;
    std::string min_value;
    int index;
    int buffersize = 0;
    int maxs = MAXSIZE;
    std::vector<std::string> in_merge_vector[maxs];
    for (int i = 0; i < maxs; i++)
    {
        in_merge_vector[i].assign(k, std::string(stringsize, ' ' ));
        
    }
    if (n == 1)
    {
        std::ifstream file1("temp.0.0");
        std::ofstream file2(output);
        while (getline(file1, text))
        {
            file2 << text << '\n';
        }
        file1.close();
        file2.close();
    }
    std::vector<int> ind(k, 0);
    std::vector<int> maxind(k, 0);
    bool breaking1 = false;
    int file_number;

    while (n != 1)
    {

        file_number = 0;
        while (file_number < n)
        {
            FilesToOpen = 0;


            ind.assign(std::min(n - file_number, k), 0);
            maxind.assign(std::min(n - file_number, k), 0);
            while (FilesToOpen < std::min(n - file_number, k))
            {
                files[FilesToOpen].open("temp." + std::to_string(merge_number) + "." + std::to_string(FilesToOpen + file_number), std::ios::binary);
                length = 0;
                while (length < MAXSIZE && getline(files[FilesToOpen], text))
                {
                    in_merge_vector[maxind[FilesToOpen]][FilesToOpen] = text;
                    maxind[FilesToOpen]++;
                    length += 1;
                }


                FilesToOpen += 1;
            }


            if (n <= k)
            {
                file_next.open(output);
            }
            else
            {
                file_next.open("temp." + std::to_string(merge_number + 1) + "." + std::to_string(file_number / k));
            }

            while (1)
            {
                empty_index = -1;
                min_value = "";
                index = 0;
                length = 0;
                while (1)
                {
                    length = 0;
                    index = 0;
                    while (ind[length] == maxind[length] && length < ind.size())
                    {
                        length += 1;
                    }
                    if (length >= ind.size())
                    {
                        break;
                    }
                    min_value = in_merge_vector[ind[length]][length];
                    index = length;
                    while (length < ind.size())
                    {

                        if (ind[length] != maxind[length])
                        {
                            if (min_value.compare(in_merge_vector[ind[length]][length]) > 0)
                            {
                                min_value = in_merge_vector[ind[length]][length];
                                index = length;
                            };
                        }
                        length += 1;
                    }

                    buffer << min_value << "\n";
                    buffersize += 1;
                    if (buffersize >= MAXSIZE)
                    {
                        buffersize = 0;
                        file_next << buffer.str();
                        buffer.str(std::string());
                    }

                    ind[index]++;
                    if (ind[index] == maxind[index])
                    {
                        empty_index = index;

                        break;
                    }
                }
                length = 0;

                file_next << buffer.str();
                buffer.str(std::string());
            
                if (empty_index >= 0)
                {

                    length = 0;
                    ind[empty_index] = 0;
                    maxind[empty_index] = 0;
                    while (length < MAXSIZE && getline(files[empty_index], text))
                    {
                        in_merge_vector[maxind[empty_index]][empty_index] = text;
                        maxind[empty_index]++;
                        length += 1;
                    }
                }

                breaking1 = true;
                for (int i = 0; i < maxind.size(); i++)
                {
                    if (ind[i] != maxind[i])
                    {
                        breaking1 = false;
                        break;
                    }
                }
                if (breaking1)
                    break;
            }

            file_next.close();
            FilesToOpen = 0;
            while (FilesToOpen < std::min(n - file_number, k))
            {
                files[FilesToOpen].close();
                FilesToOpen += 1;
            }
            file_number += k;
        }

        n = (int)ceil(float(n) / float(k));
        merge_number += 1;
        if (merge_number == num_merges)
        {
            break;
        }
    }

    return 0;
};

