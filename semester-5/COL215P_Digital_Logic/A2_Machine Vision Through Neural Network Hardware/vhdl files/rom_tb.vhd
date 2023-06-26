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

entity rom_tb is
--  Port ( );
end rom_tb;

architecture Behavioral of rom_tb is

component rom 
port (
  addr : in std_logic_vector(15 downto 0); 
  dout : out std_logic_vector(7 downto  0) 
    
)   ;
end component ;



constant clk_hz : integer := 100e6 ; 
constant clk_period :time := 1 sec / clk_hz;

signal addr : std_logic_vector(15 downto 0) ;
signal clk : std_logic := '1' ;
signal dout : std_logic_vector(7 downto  0) ;

begin
    clk <= not clk after clk_period / 2 ;
    
    dut : rom port map(addr , dout);

        SEQUENCER_PROC : process
    begin 
        wait for clk_period *2  ;
  
        addr <= "0000000011001010";   
        
          
        wait for clk_period * 2 ; 

        addr <= "0000000011001011";
        
        
        
        wait for clk_period * 2 ; 

        addr <= "0000000011001100";   
        
          
        wait for clk_period * 2 ; 
        
        addr <= "0000000011100110";
         
        wait for clk_period * 2 ; 
        
        addr <= "0000001011001111";     
 





-- for weight - biases 


        wait for clk_period *2  ;
  
        addr <= "0000010000000000";   
        
          
        wait for clk_period * 2 ; 

        addr <= "0000010000000001";
        
        
        
        wait for clk_period * 2 ; 

        addr <= "1001001101001110";   
        
          
        wait for clk_period * 2 ; 
        
        addr <= "1100011101010000";
         
        wait for clk_period * 2 ; 
        
        addr <= "1100101011001001"; 



           
    wait ;                                     
    
    end process ; 

end Behavioral;
