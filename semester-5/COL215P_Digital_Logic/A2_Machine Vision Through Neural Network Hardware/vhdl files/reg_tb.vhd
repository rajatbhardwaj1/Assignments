----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 16.09.2022 22:46:01
-- Design Name: 
-- Module Name: rom_tb - Behavioral
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

entity reg_tb is
--  Port ( );
end reg_tb;

architecture Behavioral of reg_tb is

component reg 
port (
        clk : in std_logic; 
        we : in std_logic; 
        re : in std_logic; 
        din : in std_logic_vector(15 downto 0) ;
        dout : out std_logic_vector(15 downto 0) 
    
)   ;
end component ;



constant clk_hz : integer := 100e6 ; 
constant clk_period :time := 1 sec / clk_hz;

signal clk : std_logic := '1'; 
signal we : std_logic;
signal re : std_logic;
signal din : std_logic_vector(15 downto 0); 
signal dout : std_logic_vector(15 downto 0); 

begin
    clk <= not clk after clk_period / 2 ;
    
    dut : reg port map(clk , we , re , din , dout );

        SEQUENCER_PROC : process
    begin 
        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0000000000011010";
        wait for clk_period  ;



    we <= '0' ; 
    re <= '1'; 
        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0000011110011110";
        wait for clk_period  ;

    we <= '0' ; 
    re <= '1'; 
        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0000000000011010";
        wait for clk_period  ;

    we <= '0' ; 
    re <= '1'; 
        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0000000110011010";
        wait for clk_period  ;

    we <= '0' ; 
    re <= '1'; 
        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0000000001011010";
        
          
    wait for clk_period  ;

    we <= '0' ; 
    re <= '1';  



        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0010000100011010";
        wait for clk_period  ;



    we <= '0' ; 
    re <= '1'; 
        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0100011110011110";
        wait for clk_period  ;

    we <= '0' ; 
    re <= '1'; 
        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0000011000011010";
        wait for clk_period  ;

    we <= '0' ; 
    re <= '1'; 
        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0011000000011010";
        wait for clk_period  ;

    we <= '0' ; 
    re <= '1'; 
        wait for clk_period  ;

    we <= '1' ; 
    re <= '0'; 
    din <= "0000001110011010";
        
          
    wait for clk_period  ;

    we <= '0' ; 
    re <= '1'; 
    
    wait ;                                     
    
    end process ; 

end Behavioral;
