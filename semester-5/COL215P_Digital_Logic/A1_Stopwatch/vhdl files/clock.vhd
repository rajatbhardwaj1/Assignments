

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


USE ieee.numeric_std.ALL;



entity clock is
    port(
        clk : in std_logic ;
        enable_watch : in std_logic; 
        disable_watch :in std_logic ;
        reset : in std_logic;
        seg : out std_logic_vector(6 downto 0); 
        test : out std_logic ; 
        dp: out std_logic; 
        an : out std_logic_vector(3 downto 0)
    );
   
end clock;

architecture Behavioral of clock is
CONSTANT clk_period : TIME := 10000 sec ;
    component debouncer is 
      port(
            clk : in std_logic; 
          reset : in std_logic; 
          start : in std_logic ;
          pause : in std_logic; 
          enable : out std_logic; 
          restart : out std_logic 
          
      );
    end component ;
    component clk10sec is 
    port (
        clk : in std_logic;
        en : in std_logic; 
        rst : in std_logic ;
        clk_out : out std_logic
    );
    end component ; 
    component clkmin is 
    port (
        clk : in std_logic;
         en : in std_logic; 
         rst : in std_logic ; 
       clk_out : out std_logic
    );
    end component ;
    component clksec is 
    port (
        clk : in std_logic;
          en : in std_logic; 
              rst : in std_logic ; 
      clk_out : out std_logic
    );
    end component ;
    component clktenthsec is 
    port (
        clk : in std_logic;
        en : in std_logic; 
            rst : in std_logic ; 
        clk_out : out std_logic
    );
    end component ;
    component seven_seg_decoder is 
    port (
           v : in std_logic_vector(3 downto 0);
           res : out std_logic_vector(6 downto 0)
   
   
       
       );
       end component ; 
       
     component timing_circuit is 
     port (
             clk : in std_logic;
             anode : out std_logic_vector(3 downto 0);
             s : out std_logic_vector(1 downto 0);
             helperdp : out std_logic 
     
             
         );
     end component ;
     component mux_4_to_1 is 
      port (
            v1 :    in std_logic_vector(3  downto 0);
            v2 :    in std_logic_vector(3  downto 0);
            v3 :    in std_logic_vector(3  downto 0);
            v4 :    in std_logic_vector(3  downto 0);
            s :    in std_logic_vector(1 downto 0);
            cathode :   out std_logic_vector(3 downto 0 )
    
        );
     end component ;

     component modulo_1e7 is 
     port (
             clk : in std_logic;
             q : out std_logic 
             
         );
     end component ;
     component modulo_10 is 
     port (
             clk : in std_logic;
             rst : in std_logic; 
             q : out integer range 0 to 15 
             
         );
     end component ; 
     component sec10 is 
     port (
             clk : in std_logic ; 
                          rst : in std_logic; 

             q : out integer range 0 to 15        ) ;
     end component ; 
     
 
   
     component secby10 is 
     port (
         clk : in std_logic;
                      rst : in std_logic; 

         q : out  integer range 0 to 15 );
     end component  ; 
     component clock_250hz is 
     port (
        clk : in std_logic; 
        q : out std_logic 
     );
    end component ;

    SIGNAL min :integer range 0 to 15 := 8 ;
    
    SIGNAL seco10 :  integer range 0 to 15 :=9;     
    SIGNAL sec : integer range 0 to 15 := 0 ;                                        
        SIGNAL secb10 : integer range 0 to 15 := 7;
    SIGNAL clk_10hz : std_logic ; 
    SIGNAL clk_250 : std_logic ; 
    signal v1 :    std_logic_vector(3  downto 0) := "0001";  --10th of sec
    signal v2 :     std_logic_vector(3  downto 0):= "0010";     -- sec
    signal v3 :     std_logic_vector(3  downto 0):= "0011";   -- sec * 10 
    signal v4 :     std_logic_vector(3  downto 0):= "0101";    -- min 
    signal seg_inp1: std_logic_vector(3 downto 0);  
    signal s: std_logic_vector(1 downto 0) := "01";
    signal anode : std_logic_vector(3 downto 0) := "1101";
    signal cathode : std_logic_vector(3 downto 0):= "0111";
    signal display : std_logic_vector(6 downto 0); 
    signal clkby10sec : std_logic;
    signal clksecond : std_logic;
    signal clk10second : std_logic;
    signal clkminute : std_logic;
    signal en : std_logic := '0';
    signal rst: std_logic := '0' ;
    signal testen : std_logic := '1' ;
    signal testdis : std_logic := '0' ;
    signal testrst : std_logic := '1' ;
    signal helperdp : std_logic := '0' ; 
    
    
begin

    

    

    -- if(falling_edge())
    
    
    
    clk10sec_port : clk10sec
    port map(clk  ,en,rst,  clk10second) ;

    clkmin_port : clkmin
    port map(clk10second  ,en,rst, clkminute);

    clksec_port : clksec
    port map(clk ,en , rst , clksecond );

    clktenthsec_port : clktenthsec
    port map(clk , en , rst , clkby10sec);





    modulo_secby10_port : secby10 
    port map(clkby10sec,rst  , secb10);
    
    modulo_10_port : modulo_10 
    port map(clksecond ,rst ,sec) ;


    modulo_sec10_port : sec10
    port map(clk10second , rst , seco10); 


    min_port : modulo_10 
    port map( clkminute ,rst, min) ;


    clock_250hz_port : clock_250hz
    port map(clk , clk_250) ;


    time_cir_port: timing_circuit
    port map(clk_250 ,anode , s , helperdp  ) ;

    
    
    mux: mux_4_to_1 
    port map(v1,v2,v3,v4,s , cathode ) ; 
 
    
    seven_seg_decoder_port : seven_seg_decoder
    port map(cathode , display); 
    
    debouncer_port : debouncer
    port map(clk , testrst , testen , testdis , en , rst) ;

      SEQUENCER_PROC : PROCESS
        BEGIN
            dp <= helperdp ;
            v1 <= std_logic_vector(to_unsigned(secb10, 4));
            v2 <= std_logic_vector(to_unsigned(sec, 4));
            v3 <= std_logic_vector(to_unsigned(seco10, 4));
            v4 <= std_logic_vector(to_unsigned(min, 4));
            seg <= display ;
            test <= rst ; 
            
            an <= anode ;
            testen <= enable_watch ;
            testdis <= disable_watch ; 
            testrst <= reset  ;
            report "The value of 'a' is " & integer'image(min);
            WAIT FOR clk_period ;
            WAIT;
        END PROCESS;
   
    
end Behavioral;