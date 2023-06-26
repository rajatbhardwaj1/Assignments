----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 26.08.2022 15:17:32
-- Design Name: 
-- Module Name: clock_250hz - Behavioral
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
use ieee.numeric_std.all;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity clock_250hz is
  Port ( 
  clk : in std_logic ; 
  q : out std_logic 
  );
end clock_250hz;

architecture Behavioral of clock_250hz is
signal count :  unsigned(256 downto 0) ;
signal ct : integer := 1; 
signal op : std_logic:= '0';
begin
    process(clk)
    begin 
   
    if(rising_edge(clk)) then 
    ct <= ct + 1 ;
    if (ct = 250000) then 
    op <= not op ;
    ct <= 0 ;
    end if ;
    end if ;

     
    q <= op ; 
    
    end process ;


end Behavioral;