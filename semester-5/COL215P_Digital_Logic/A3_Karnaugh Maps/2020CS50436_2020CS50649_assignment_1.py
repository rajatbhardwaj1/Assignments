from cgitb import grey
from K_map_gui_tk import *

import math

def int_to_bin(inp , size):
    s = ''
    for i in range(size) :
        if 1<<i & inp > 0 :
            s = '1' + s
        else :
            s =  '0' + s
    return s 


def int_list_to_bin(l):
    size = len(l) 
    p = 0 
    while p**2 < size :
        p += 1 
    ans  = []
    for element in l :
        ans.append(int_to_bin(element, p) )
    return ans 

def grayCode(n):
        l = [0,1] 
        base = 1 
        if n == 1 :
            return ['0','1'] 
        while n > 1 :
            temp = []
            for i in l :
                temp.append(i)
            temp.reverse() 
            temp = l + temp 
            for i in range(len(temp)// 2 , len(temp)):
                temp[i] = 2**base + temp[i] 
                
                
            base += 1
            l = temp 
            n -= 1 
        return int_list_to_bin(l) 

    
def is_legal_region(kmap_function, term):
    n = len(kmap_function)
    m = len(kmap_function[0])
    sz = len(term)
   
    l1 = grayCode(n//2)
    l2 = grayCode(m//2)
    
    n1 = math.floor(math.log2(n))
    m1 = math.floor(math.log2(m))

    temp1=[]
    temp2=[]
    new_term=[]

    for i in range(m1):
        temp1.append(term[i])
    
    for i in range(m1,m1+n1):
        temp2.append(term[i]) 

    new_term = temp2 + temp1
    Dict = {}
    for i in range(n) :
        for j in range(m) :
            Dict[l1[i]+l2[j]] = (kmap_function[i][j],(i,j))

    flag = True
    region = []
    for key in Dict : 
        temp = True 
        for i in range(sz):
            if(new_term[i]!=None and new_term[i]!=(ord(key[i])-48)):
                temp = False 
        if(temp):
            val, coor = Dict[key]
            region.append(coor)
            if(val==0):
                flag = False
            
    if(n==4 and m==4) :

        if(len(region)==1):
            return (region[0], region[0], flag)
        elif (len(region)==2):
            if((region[0]==(0,0) or region[0]==(0,3) or region[0]==(3,0) or region[0]==(3,3)) and (region[1]==(0,0) or region[1]==(0,3) or region[1]==(3,0) or region[1]==(3,3))):
                return (region[1],region[0],flag)
            else:
                return (region[0],region[1],flag)
        elif (len(region)==4):
            if(region[0]==(0,0) and region[1]==(0,3) and region[2]==(3,0) and region[3]==(3,3)):
                return ((3,3), (0,0), flag)
            elif (region[0][0]==0 and region[1][0]==0 and region[2][0]==3 and region[3][0]==3):
                return (region[2],region[1],flag)
            elif (region[0][1]==0 and region[1][1]==3 and region[2][1]==0 and region[3][1]==3):
                return (region[1],region[2],flag)
            else:
                return (region[0],region[3],flag)
        elif (len(region)==8):
            if(region[0][0]==0 and region[1][0]==0 and region[2][0]==0 and region[3][0]==0 and region[4][0]==3 and region[5][0]==3 and region[6][0]==3 and region[7][0]==3):
                return (region[4],region[3],flag)
            elif(region[0][1]==0 and region[1][1]==3 and region[2][1]==0 and region[3][1]==3 and region[4][1]==0 and region[5][1]==3 and region[6][1]==0 and region[7][1]==3):
                return (region[1],region[6],flag)
            else:
                return (region[0],region[7],flag)
        else:
            return ((0,0), (3,3), flag)

    elif(n==2 and m==4):
        if(len(region)==1):
            return (region[0],region[0],flag)
        elif(len(region)==2):
            if((region[0]==(0,0) and region[1]==(0,3)) or (region[0]==(1,0) and region[1]==(1,3))):
                return (region[1],region[0],flag)
            else:
                return (region[0],region[1],flag)
        elif(len(region)==4):
             return (region[0],region[3],flag)
        else:
            return (region[0],region[7],flag)

    elif(n==2 and m==2):
        if(len(region)==1):
            return (region[0],region[0],flag)
        elif(len(region)==2):
            return (region[0],region[1],flag)
        else:
            return (region[0],region[3],flag)

    else :
        return ((0,0),(0,0),flag)

def main():

    (x1,y1), (x2,y2), flag = is_legal_region([[None,0,None,1],[None,0,None,0],[None,1,None,None],[1,None,1,0]],[None,0,0,None])
    print((x1,y1), (x2,y2), flag)
    root = kmap([[None,0,None,1],[None,0,None,0],[None,1,None,None],[1,None,1,0]])
    root.draw_region(x1,y1,x2,y2,"blue")
    root.mainloop()



main()