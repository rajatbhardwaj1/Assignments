library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity modulo_10 is
    port (
        clk : in std_logic;
        rst : in std_logic; 
        q : out integer range 0 to 15 
        
    );
end modulo_10;

architecture beh of modulo_10 is
signal count :  integer range 0 to 15 ;
begin

   
   process(clk,rst )
    begin 
    
     if(rst = '1') then 
     count <= 0 ; 
     elsif(rising_edge(clk)) then 
    count <= count + 1 ;
    if(count = 9 ) then 
        count <= 0; 
        end if ;

    end if ; 
    
    if(rising_edge(rst)) then
         
    end if ;

   
    
    end process ;
        q <= count ;




end beh ;