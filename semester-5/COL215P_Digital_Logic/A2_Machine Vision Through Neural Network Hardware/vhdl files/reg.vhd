----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 16.09.2022 13:58:26
-- Design Name: 
-- Module Name: reg - Behavioral
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

entity reg is
  Port (
        clk : in std_logic; 
        we : in std_logic; 
        re : in std_logic; 
        din : in std_logic_vector(15 downto 0) ;
        dout : out std_logic_vector(15 downto 0) 
   );
end reg;

architecture Behavioral of reg is

signal register_file : std_logic_vector(15 downto 0);

begin
   
    process(clk)
        begin 
        if(rising_edge(clk)) then
            if(re='1') then
                dout <= register_file ; 
            end if;
            if(we='1') then
                register_file <= din;
            end if;
        end if ;
    end process ;

end Behavioral;
