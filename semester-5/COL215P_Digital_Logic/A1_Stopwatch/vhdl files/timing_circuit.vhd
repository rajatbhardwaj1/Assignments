library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity timing_circuit is
    port (
        clk : in std_logic;
        anode : out std_logic_vector(3 downto 0);
        s : out std_logic_vector(1 downto 0);
        helperdp :out std_logic 

        
    );
end timing_circuit;

architecture beh of timing_circuit is
    signal count : unsigned(63 downto 0);
    signal anodehelper : std_logic;

    begin 
        process(clk)
        begin
            if(rising_edge(clk)) then

                count <= count + 1; 

            end if ;

            if(count rem  4 =  0) then 
                anode <= "1110"; 
                s <= "00";
                helperdp <='1'; 

            elsif (count rem 4 = 1 )then
                anode <= "1101"; 
                s <= "01";
                helperdp <= '0' ; 


            elsif (count rem 4 = 2 )then
                anode <= "1011"; 
                s <= "10";
                helperdp <='1'; 

            elsif (count rem 4 = 3 )then
                anode <= "0111"; 
                s <= "11";
                helperdp <= '0' ; 



                
            end if ;

        
        end process; 




end architecture;