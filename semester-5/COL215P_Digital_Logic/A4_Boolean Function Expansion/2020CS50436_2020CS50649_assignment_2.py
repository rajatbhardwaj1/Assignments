#this function converts a list to string . Ex - ["a" , "b'", "c" , "d'"] is converted to "ab'cd'"
from array import array
import copy
import string
from symbol import funcdef



def convert_list_to_string (L):
    s = ""
    for i in L:
        s += i 
    return s 



#this function converts a string of literals to a list . Ex - "ab'cd'" is converted to ["a" , "b'", "c" , "d'"]
def convert_string_to_list(s : string):
    ans = [] 
    skip = False 
    
    for i in range(len(s)):
        if not skip:
            if i < len(s) - 1:
                if s[i+1] == "'":
                    skip = True 
                    ans.append(s[i] + s[i+1])
                   
                else :
                    ans.append(s[i])
            else :
                ans.append(s[i])
        else :
            skip = False 
    return ans 


#this functions finds all possible regions which are double in size of s and also the (region - s) term 
def doubleregion(s):
    ans = [] 
    sl = convert_string_to_list(s)
    for i in range(len(sl)) :
        s1 = copy.deepcopy(sl)
        s1_neg = copy.deepcopy(sl)
        if len(s1[i]) == 2:
            s1_neg[i] = str(s1[i][0])
        else :
            s1_neg[i] = str(s1[i]+"'") 
        s1.remove(sl[i])
        ans.append((convert_list_to_string(s1) , convert_list_to_string(s1_neg)))
    return ans 

#This function builds a set of legal regions 

def insert(regionset , F , legal_regions ):
    if len(F) < 1:
        return   
    newF = set()  
    removal_terms = set()
    for term in F:
        regionset.add(term)
        potential_terms = doubleregion(term)
        for expansion , negterm in potential_terms:
            if negterm in regionset:
                removal_terms.add(term)
                removal_terms.add( negterm)
                newF.add(expansion)

    for term in F:
        if term not in removal_terms:
            legal_regions.append(term)
        else :
            regionset.remove(term)
    insert(regionset , newF , legal_regions)

def fallin(term , reg):
    l1 = convert_string_to_list(reg)
    l2 = convert_string_to_list(term)
    for i in l1 :
        if i not in l2 :
            return False 
    return True 



def comb_function_expansion(func_TRUE, func_DC):
    regionset = set()
    doubleset = set() 
    answer = [] 
    F = set(func_TRUE + func_DC)
    maxlen = 2**(len(func_TRUE[0]) - func_TRUE[0].count("'") ) 

    #hardcoded 


    if len(func_TRUE) == 0 :
        return [] 
    if maxlen == len(F) :
        return [None]*len(func_TRUE)

    legal_regions = [] 
    insert(regionset , F , legal_regions )
    legal_regions.reverse()
    for term in func_TRUE:
        reg = term
        final_len = len(reg) - reg.count("'")
        for region in legal_regions:
            if fallin(term , region):
                new_len = len(region) - region.count("'")
                if new_len< final_len:
                    final_len = new_len 
                    reg = region
                break
        answer.append(reg)

    return answer


#for finding the next legal region and for finding the terms it combines with from the input
def print_future(term , funcTrue , FuncDC):
    F = set(funcTrue + FuncDC)
    drs = doubleregion(term)
    req = 2**((len(funcTrue[0]) - funcTrue[0].count("'"))  - (len(term) - term.count("'")))

    for exp , neg in drs : 
        answerset = [] 
        ct = 0 
        for termn in F:
            if fallin(termn , exp) and not fallin(termn , term) :
                answerset.append(termn)
                ct += 1 
            if ct == req :
                print("Terms to be combined with are")
                print(answerset)
                print("Next legal region is ")
                print(exp)
                break
        

    
   