library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity clkmin is
    port (
        clk : in std_logic; -- 10 sec 
        en : in std_logic ;
        rst : in std_logic ; 
        clk_out : out std_logic
    );
end clkmin;



architecture beh of clkmin is
  signal ct : integer := 0; 
  signal op : std_logic:= '1';
begin

   process(clk)
    begin 
    
     if(rst = '1') then
       ct <= 0 ;
       op <= '1' ;
       
       elsif(rising_edge(clk) and en = '1' ) then 
    ct <= ct + 1 ;
    if(ct = 2) then 
    op <= not op ; 
    ct <= 0 ; 
    end if ;

    end if ; 
    clk_out <= op ; 
    
    end process ;

end beh;