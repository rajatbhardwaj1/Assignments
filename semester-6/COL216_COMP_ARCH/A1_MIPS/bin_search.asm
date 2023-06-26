.data
    size_prompt     : .asciiz "Enter the Total number of integers in the array: "
    array_prompt    : .asciiz "Enter the elements of the array:\n"
    x_prompt        : .asciiz "Enter the value of x: "
    x_found         : .asciiz "Yes at index "
    x_not_found     : .asciiz "Not found"

.text
    main:
            # user prompt to enter number of integers in the array

            li      $v0, 4                  # syscall to print the string 
            la      $a0, size_prompt        # starting address of the string to be stored at $a0 for syscall
            syscall     

            # taking an integer input from the user 
            li		$v0, 5		            # $v0 = 5 i.e syscall to read an integer 
            syscall                         # the entered int is also stored in $v0
            move    $t0, $v0                # moving the size of the array in $t0

            # prompting the user to enter the array
            li      $v0, 4                  # syscall to print the string 
            la      $a0, array_prompt       # starting address of the string to be stored at $a0 for syscall
            syscall          

            move    $t1 , $t0               # $t1 = n , i.e. the size of the string is stored in t1
            move    $s0 , $t1

            #initializing heap for the input 
            li      $v0 , 9
            li      $a0 , 1000              #allocating heap memory, the address of heap is stored at $v0
            syscall
            move    $a0 , $v0               # $a0 = address of the heap to be used for input 
            move    $t0 , $v0               # $t0 = address of the heap 



            # taking the array input in loop
inp_loop:   
            beq		$t1, $zero, inp_end	    # if $t1 == $zero then goto inp_end (no more input required , therefore exit loop)
            li      $v0, 5                  # syscall to read integer and store it at $a0
            syscall                         # the input integer is stored in $v0 we need to store it at the address of $a0
            sw		$v0, 0($a0)		        # The int is stored at $a0
            addi	$a0, $a0, 4			    # incrementing the storing address by size of an int 
            addi	$t1, $t1, -1			# decrementing the number of integers to be stored
            j		inp_loop				# loop to get input again
            


     

inp_end:    
            # user prompt to enter value of x
            li      $v0, 4                  # syscall to print the string 
            la      $a0, x_prompt           # starting address of the string to be stored at $a0 for syscall
            syscall     


            # Taking the input x
            li		$v0, 5		
            syscall
            move    $t3, $v0                # $t3 = x


            # initializing the left and right values for binary search
            addi    $t4 , $zero , 0         # $t4 = left = 0  
            addi    $t5 , $s0 , -1          # $t5 = right = n - 1 (i.e. the last index) 
            j       bin_loop

            # binary search here 
bin_loop:   
            slt     $t6, $t5 , $t4          # if right < left then we store 1 in $t6 else we store 0 in t6 
            bne     $t6, $zero, bin_not_found    # if right < left then we end the loop , i.e. $t6 != 0 
            add     $t7, $t4, $t5           # $t7 = mid = left + right
            srl     $t7, $t7, 1             # $t7 = mid = (left + right)/2
            mul     $t7 , $t7 , 4           # $t7 = offset of mid
            add     $t6, $t0 , $t7          # $t6 has the address of mid now    t6 = address of array($t0) + offset ($t7)
            lw      $t8, ($t6)              # $t8 = arr[mid] 
            beq     $t8 , $t3, bin_found    # if(arr[mid] ($t8) == x ($t3) ) then we jump to found branch 
            slt     $t6 , $t8 , $t3         # if arr[mid] < x) then we store 1 at t6
            beq     $t6 , $zero , right     # if(arr[mid] > x) then  jump to modify right = mid - 1 
            bne     $t6 , $zero , left      # if(arr[mid] < x) then  jump to modify left = mid - 1 



            # binary search on the left half 
right:  
            div     $t7 , $t7, 4            # adjusting the 4*mid to mid by dividing it by 4
            addi    $t7 , $t7 , -1          # making mid = mid - 1
            move    $t5 , $t7               # right = mid - 1 
            j    bin_loop


            #binary search on the right half 
left:       div     $t7 , $t7, 4            # adjusting the 4*mid to mid by dividing it by 4
            addi    $t7 , $t7 , 1           # making mid = mid + 1 
            move    $t4 , $t7               # left = mid + 1
            j    bin_loop




bin_not_found:
            # prompting the user to enter the array
            li      $v0, 4                  # syscall to print the string 
            la      $a0,  x_not_found      
            syscall                         # printing not found 
            j   finished                    # exiting program


bin_found : 
            # prompting the user to enter the array
            li      $v0, 4                  # syscall to print the string 
            la      $a0,   x_found          
            syscall                         # printing  Yes at index 
            div     $t7 , $t7 , 4           # making mid * 4 = mid by dividing it by 4
            move    $a0 , $t7               # moving mid to $a0 for syscall to print the int
            li      $v0,  1                 # syscall to print integer 
            syscall                         # printing mid
            j   finished                    # exiting program



finished:   
            # end of main function 
                li $v0 , 10
                syscall