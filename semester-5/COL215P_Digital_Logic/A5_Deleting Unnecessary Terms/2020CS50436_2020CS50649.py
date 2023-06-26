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


def find_essentialPI(term_reg):
    ans = []
    for term in term_reg:
        if len(term_reg[term]) == 1 : 
            if term_reg[term][0] not in ans :
                ans.append(term_reg[term][0])
    return ans 



            


def check(term , EssentialPI):
    for EPI in EssentialPI :
        if EPI in term :
            return True
    return False 

def demo(EssentialPI ,term_reg):
    num = 1 
    for term in term_reg:
        head = ""
        for i in term_reg[term]:
            if i in EssentialPI :
                head = i 
                break
        for i in term_reg[term]:
            if i not in EssentialPI:
                print(f"{num}.\nterm :{term} \nRegion to be deleted : {i}\nRegion covering it : {head}\n\n")
                num += 1 
                


def opt_function_reduce(func_TRUE, func_DC):
    regionset = set()
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
    term_reg = {}
    for term in func_TRUE:
        for region in legal_regions:
            if fallin(term , region):
                
                try:
                    term_reg[term].append(region) if region not in term_reg[term] else None # Add minterm in chart
                except KeyError:
                    term_reg[term] = [region]

    # print(term_reg)
    EssentialPI = find_essentialPI(term_reg)
    Remaining = {}
    for term in term_reg:
        inEPI = False 
        for EPI in EssentialPI : 
            if EPI in term_reg[term]:
                inEPI = True
                break
        if not inEPI:
            Remaining[term] = term_reg[term]
   
    if len(Remaining) == 0:
        # demo(EssentialPI , term_reg)
        return EssentialPI 
    else :   
        for term in Remaining:
            temp = [] 
            if not check(Remaining[term] , EssentialPI):
    
                EssentialPI.append(Remaining[term][0])
        # demo(EssentialPI , term_reg)
        return EssentialPI

        
func_TRUE = ["a'b'c'd'e'fg", "a'bc'd'e'fg'", "abc'd'efg'", "ab'c'd'efg'", "abc'defg'", "abcdefg'", "a'bcdefg''", "a'bcd'ef'g'", "abcd'e'fg", "a'bc'defg", "abc'defg", "abcdefg",  "a'bcdefg", "a'bcd'efg", "abcd'efg", "a'b'cd'efg", "ab'cd'efg", "abcdef'g",  "a'bcdef'g", "a'bcd'ef'g", "abcd'ef'g", "a'b'cd'efg'", "ab'cd'efg'" ] 
func_DC = []  
print(opt_function_reduce(func_TRUE , func_DC))

   