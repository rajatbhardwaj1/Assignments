----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 16.09.2022 13:09:31
-- Design Name: 
-- Module Name: ram - Behavioral
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
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity ram is
  Port (
        clk : in std_logic ; 
        din : in std_logic_vector(15 downto 0 ) ;
        addr : in std_logic_vector(10 downto 0) ;
        we : in std_logic ;
        re : in std_logic ;
        dout : out std_logic_vector (15 downto 0)
  );
end ram;

architecture Behavioral of ram is
type memory is array(0 to 1023) of std_logic_vector(15 downto 0);
signal tmp : std_logic_vector(15 downto 0) ;
signal mem : memory ; 
begin
    process(clk)
        begin 
        if(rising_edge(clk)) then
            if(re='1') then
                tmp <= mem(to_integer(unsigned(addr)));
            end if;
            if(we='1') then
                mem(to_integer(unsigned(addr))) <= din;
            end if;
        end if ;
    end process ;
    
    dout <= tmp;

end Behavioral;
