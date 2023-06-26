library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity comparator_tb is
Generic (
		PERIOD : time := 10 ns
    );
end comparator_tb;

architecture tb of comparator_tb is
component comparator
Port ( 
      din : in std_logic_vector(15 downto 0);
      dout : out std_logic_vector(15 downto 0)
  );
end component;

signal din,dout : std_logic_vector(15 downto 0);
begin

uut: comparator port map(din,dout);

stimulus: process
begin
din <= x"feed";
wait for PERIOD;
din <= x"0eed";
wait for PERIOD;
din <= x"8524";
wait for PERIOD;
din <= x"1234";
wait for PERIOD;
din <= x"eda1";
wait for PERIOD;
wait;
end process;
end tb;
