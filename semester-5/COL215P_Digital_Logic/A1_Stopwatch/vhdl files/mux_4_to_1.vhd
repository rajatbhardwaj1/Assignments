library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity mux_4_to_1 is
    port (
        v1 :    in std_logic_vector(3  downto 0);
        v2 :    in std_logic_vector(3  downto 0);
        v3 :    in std_logic_vector(3  downto 0);
        v4 :    in std_logic_vector(3  downto 0);
        s :    in std_logic_vector(1 downto 0);
        cathode :   out std_logic_vector(3 downto 0 )

    );
end mux_4_to_1;

architecture beh  of mux_4_to_1 is
begin
    process(v1 , v2 , v3 , v4, s ) is 
    begin
            if(s(0) = '0' and s(1) = '0') then 
            cathode <= v1 ;
            elsif(s(0) = '1' and s(1) = '0') then 
            cathode <= v2 ;
            elsif(s(0) = '0' and s(1) = '1') then 
            cathode <= v3 ;
            else 
            cathode <= v4 ;
            end if ;
    end process ;
end beh;