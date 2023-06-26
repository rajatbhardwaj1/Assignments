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

entity shifter_tb is
--  Port ( );
end shifter_tb;

architecture Behavioral of shifter_tb is

constant clk_hz : integer := 100e6 ; 
constant clk_period :time := 1 sec / clk_hz;
signal clk : std_logic := '1' ;

signal din: std_logic_vector(15 downto 0);
signal dout: std_logic_vector(15 downto 0);

signal en: std_logic;

component shifter 
 Port ( 
        clk : in std_logic; 
        en : in std_logic ;
        din : in std_logic_vector(15 downto 0);
        dout : out std_logic_vector(15 downto 0)
 );
 end component;
begin
    clk <= not clk after clk_period / 2 ;
    
        dut : shifter port map(clk , en  , din , dout);

    SEQUENCER_PROC : process
    begin 
        wait for clk_period  ;
    din <= "0101010101010111";
    en <= '1' ; 
    
    
    
        wait for clk_period  ;
    din <= "0101000001010111";
    en <= '1' ; 
    
    
    
        wait for clk_period  ;
    din <= "0111110101010111";
    en <= '1' ; 
    
    
    
        wait for clk_period  ;
    din <= "0101010101110111";
    en <= '1' ; 
    
    
    
        wait for clk_period  ;
    din <= "0000010101010111";
    en <= '1' ; 
    
    
        wait for clk_period  ;
    din <= "0101010100000100";
    en <= '1' ; 
    
    
    
        wait for clk_period  ;
    din <= "0101011111011111";
    en <= '1' ; 
    
    
    
        wait for clk_period  ;
    din <= "0101111110100111";
    en <= '1' ; 
    
    
    
  
    end process ;

end Behavioral;
