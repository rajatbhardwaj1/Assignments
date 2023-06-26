----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 26.08.2022 14:30:48
-- Design Name: 
-- Module Name: sec10 - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity sec10 is
  Port (
        clk : in std_logic ; 
        rst : in std_logic ;
        q : out integer range 0 to 15        
   );
end sec10;

architecture Behavioral of sec10 is
signal count :  integer  := 0   ;

begin
    process(clk)
   begin 
      
     if(rst = '1') then 
        count <= 0 ; 
        elsif(rising_edge(clk)) then 
     count <= count + 1 ;
     if(count = 5 ) then 
         count <= 0; 
         end if ;
 
     end if ; 
     q <= count ;
     
     end process ;
    

end Behavioral;