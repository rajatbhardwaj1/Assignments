----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 02.09.2022 13:05:57
-- Design Name: 
-- Module Name: debouncer - Behavioral
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

entity debouncer is
--  Port ( );
    port(
        clk : in std_logic; 
        reset : in std_logic; 
        start : in std_logic ;
        pause : in std_logic; 
        enable : out std_logic; 
        restart : out std_logic 
        
    );
 
end debouncer;

architecture Behavioral of debouncer is
signal enab : std_logic := '0' ; 
signal counter : integer := 0;
signal rst : std_logic := '0' ;

signal s : std_logic := '0';
begin
    process(clk, reset )
        begin  
            if(rising_edge(clk)) then 
                if(start = '1') then 
                    enab <= '1' ;
                end if ;

                if(pause = '1') then 
                enab <= '0' ;
                end if ;
                if(reset = '1') then 
                rst <= '1' ; 
                end if ;
                if(reset = '0') then 
                rst <= '0' ;
                end if ;
                
                
                
            end if ;
       
              
            
end process ;
             enable <= enab ; 
             restart <= rst ; 
             



end Behavioral;
