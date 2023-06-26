library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity ram_tb is
Generic (
		PERIOD : time := 10 ns
    );
end ram_tb;

architecture tb of ram_tb is
component ram
port(
        clk : in std_logic ; 
        din : in std_logic_vector(15 downto 0 ) ;
        addr : in std_logic_vector(10 downto 0) ;
        we : in std_logic ;
        re : in std_logic ;
        dout : out std_logic_vector (15 downto 0)
  );
end component;

signal din,dout : std_logic_vector(15 downto 0);
signal addr : std_logic_vector(7 downto 0);
signal clk,we,re : std_logic := '0';
signal clk_generator_finish : STD_LOGIC := '0';
signal test_bench_finish : STD_LOGIC := '0';

begin

uut: ram port map(clk,din,addr,we,re,dout);

clock : process
begin
    while ( clk_generator_finish /= '1') loop
       clk <= not clk;
       wait for PERIOD/2;
    end loop;
    wait;
end process;

stimulus: process
begin
addr <= x"00";
we <= '1';
din <= x"0001";
wait for PERIOD;
addr <= x"0c";
din <= x"face";
wait for PERIOD;
addr <= x"4a";
din <= x"d12a";
wait for PERIOD;
addr <= x"11";
din <= x"1234";
wait for PERIOD;
addr <= x"87";
din <= x"8787";
wait for PERIOD;
we <= '0';
re <= '1';
addr <= x"0c";
wait for PERIOD;
addr <= x"4a";
wait for PERIOD;
addr <= x"11";
wait for PERIOD;
addr <= x"87";
wait for PERIOD;
addr <= x"00";
test_bench_finish <= '1';
clk_generator_finish <= '1';
wait for PERIOD;
wait;
end process;
end tb;