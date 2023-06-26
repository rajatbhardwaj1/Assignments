library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity seven_seg_decoder is
    port (
        v : in std_logic_vector(3 downto 0);
        res : out std_logic_vector(6 downto 0)



    );
end seven_seg_decoder;

architecture beh of seven_seg_decoder is
    begin 
        process(v)
    begin

    res(6) <=  not(v(1) or v(3) or ((not v(2)) and (not v(0))) or (v(2) and v(0))  ) ;     --y = C + A + B'D' + BD
    res(5) <=   not((not v(2)) or ((not v(1) ) and (not v(0) )) or (v(1) and v(0))) ;                     --y = B' + C'D' + CD
    res(4) <= not((not v(1)) or v(0) or v(2));                         --  y = C' + D + B
    res(3) <=  not(v(3) or ((not v(2)) and not v(0)) or ((not v(2) ) and v(1)) or (v(1) and (not v(0) )) or (v(2) and (not v(1) ) and v(0)) );           --y = A + B'D' + B'C + CD' + BC'D
    res(2) <=  not( ((not v(2) ) and (not v(0) )) or (v(1) and (not v(0))));              --y = B'D' + CD'
    res(1) <=   not(v(3) or ((not v(1)) and (not v(0))) or (v(2) and (not v(1))) or (v(2) and (not v(0))));                --y = A + C'D' + BC' + BD'
    res(0) <=   not(v(3) or ((not v(2)) and v(1)) or (v(1) and (not v(0)) )  or (v(2) and not v(1)));        --y = A + B'C + CD' + BC'

    
    end process ;

end architecture;