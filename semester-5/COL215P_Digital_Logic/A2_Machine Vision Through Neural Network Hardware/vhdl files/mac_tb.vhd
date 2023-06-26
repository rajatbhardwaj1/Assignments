----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 17.09.2022 22:15:35
-- Design Name: 
-- Module Name: mac_tb - Behavioral
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

entity mac_tb is
--  Port ( );
end mac_tb;

architecture Behavioral of mac_tb is

constant clk_hz : integer := 100e6 ; 
constant clk_period :time := 1 sec / clk_hz;
signal clk : std_logic := '1' ;

signal din1: std_logic_vector(7 downto 0);
signal din2: std_logic_vector(15 downto 0);

signal cntrl: std_logic;
signal dout:  std_logic_vector(15 downto 0);

component mac 
 Port ( 
     clk : in std_logic;

    din1: in std_logic_vector(7 downto 0);
  din2: in std_logic_vector(15 downto 0);
   cntrl: in std_logic;
   dout: out std_logic_vector(15 downto 0)
 );
 end component;
begin
    clk <= not clk after clk_period / 2 ;
        dut : mac port map(clk , din1 , din2 , cntrl, dout);

    SEQUENCER_PROC : process
    begin 
        wait for clk_period  ;
    din1 <= "00000010" ; 
    din2 <= "0000000000000101" ; 
    cntrl <= '1' ;
        wait for clk_period  ;
    din1 <= "00000011" ; 
    din2 <= "0000000000000111" ; 
    cntrl <= '0' ;
        wait for clk_period  ;
    din1 <= "00000111" ; 
    din2 <= "0000000000000011" ; 
    cntrl <= '0' ;
        wait for clk_period  ;
    din1 <= "00000001" ; 
    din2 <= "0000000000000101" ; 
    cntrl <= '0' ;
        wait for clk_period  ;
    din1 <= "00000010" ; 
    din2 <= "0000000000000001" ; 
    cntrl <= '0' ;
        wait for clk_period  ;
    din1 <= "00001010" ; 
    din2 <= "0000000000010101" ; 
    cntrl <= '1' ;
        wait for clk_period  ;
    din1 <= "00000010" ; 
    din2 <= "0000000000100101" ; 
    cntrl <= '0' ;
        wait for clk_period   ;
    din1 <= "00010010" ; 
    din2 <= "0000000000000101" ; 
    cntrl <= '0' ;
        wait for clk_period  ;
    din1 <= "00000011" ; 
    din2 <= "0000000000000111" ; 
    cntrl <= '0' ;
        
        
        
  
    end process ;

end Behavioral;
