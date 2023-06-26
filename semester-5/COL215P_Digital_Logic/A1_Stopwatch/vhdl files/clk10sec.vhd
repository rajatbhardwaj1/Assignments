library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity clk10sec is
    port (
        clk : in std_logic;
        en : in std_logic ;
        rst : in std_logic; 
        clk_out : out std_logic

    );
end clk10sec;



architecture beh of clk10sec is
   signal ct : integer := 0; 
 signal op : std_logic:= '1';
begin

    process(clk ,rst )
    begin 
    if(rst = '1') then
    ct <= 0 ;
    op<= '1';
    
    elsif(rising_edge(clk) and en = '1') then 
        ct <= ct + 1 ;
        if(ct = 499999999) then 
            op <= not op ; 
            ct <= 0 ; 
        end if ;

    end if ; 
    clk_out <= op ; 
    
    end process ;


end beh;