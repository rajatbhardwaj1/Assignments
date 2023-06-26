----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09.09.2022 13:39:20
-- Design Name: 
-- Module Name: mac - Behavioral
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

entity mac is
  Port ( 
      signal clk : in std_logic;

    signal din1: in std_logic_vector(7 downto 0);
    signal din2: in std_logic_vector(15 downto 0);
    signal cntrl: in std_logic;
    signal dout: out std_logic_vector(15 downto 0)
  );

end mac;

architecture Behavioral of mac is
signal acc: std_logic_vector(15 downto 0) := "0000000000000000";
signal temp: std_logic_vector(23 downto 0):=X"000000";


begin
process(clk)
begin
 if (rising_edge(clk)) then 
    if(cntrl='1') then 
    acc <= temp(15 downto 0);
    else
    acc <= temp(15  downto 0 )  +acc;
    end if;
end if;
end process;

dout <= acc;
temp <= std_logic_vector(signed(din1) * signed(din2));


end Behavioral;
