----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 16.09.2022 14:08:13
-- Design Name: 
-- Module Name: shifter - Behavioral
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

entity shifter is
  Port ( 
        en : in std_logic ;
        din : in std_logic_vector(15 downto 0);
        dout : out std_logic_vector(15 downto 0)
        
  );
end shifter;

architecture Behavioral of shifter is
    
signal temp : std_logic_vector(15 downto 0);
begin
  
    dout <= "00000" & din(15 downto 5) when en = '1' else X"0000";


end Behavioral;
