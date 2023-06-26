
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

entity control_logic is

    generic (
        ADDR_WIDTH : integer := 16;
        DATA_WIDTH : integer := 8;
        IMAGE_SIZE : integer := 784;
        TOTAL_ROM_SIZE : integer := 65536  ;
        WEIGHT_BAIS_SIZE : integer := 50890;
        IMAGE_FILE_NAME : string :="imgdata_digit7.mif";
        img_base : integer := 0;
        weight1_base : integer := 1024 ; 
        weight2_base : integer := 51264;  
        bais1_base : integer := 51200;
        bais2_base : integer := 51904 ;             
        WEIGHT_BIAS_FILE_NAME : string := "weights_bias.mif"
    );
    
    port(
        clk : in std_logic;
        an : out std_logic_vector(3 downto 0);
        seg : out std_logic_vector(6 downto 0)
        );
end control_logic ;



architecture beh of control_logic is 
component mac 
 Port ( 
     clk : in std_logic;

    din1: in std_logic_vector(7 downto 0);
  din2: in std_logic_vector(15 downto 0);
   cntrl: in std_logic;
   dout: out std_logic_vector(15 downto 0)
 );
 
end component ;

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


component reg 
port (
        clk : in std_logic; 
        we : in std_logic; 
        re : in std_logic; 
        din : in std_logic_vector(15 downto 0) ;
        dout : out std_logic_vector(15 downto 0) 
    
)   ;
end component ;


component rom 
port (
  clk : in std_logic; 
  addr : in std_logic_vector(15 downto 0); 
  dout : out std_logic_vector(7 downto  0) 
    
)   ;
end component ;

component shifter 
  Port ( 
      en : in std_logic ;
      din : in std_logic_vector(15 downto 0);
      dout : out std_logic_vector(15 downto 0)
      
);

end component ;

component comparator 
  Port ( 
    din : in std_logic_vector(15 downto 0);
    dout : out std_logic_vector(15 downto 0)
);
end component  ; 

component seven_seg_decoder 
   port (
     v : in std_logic_vector(3 downto 0);
     res : out std_logic_vector(6 downto 0)

 );

end component ; 

--signals 




constant clk_hz : integer := 100e6 ; 
constant clk_period :time := 1 sec / clk_hz;
--signal clk : std_logic := '1' ;
signal ctrl : std_logic  := '1';
signal img_addr : std_logic_vector(15 downto 0)  ;
signal weight_addr : std_logic_vector(15 downto 0) ;
signal bias_addr :  std_logic_vector(15 downto 0) ;
signal mac_inp1 : std_logic_vector(7 downto 0) ; 
--signal mac_inp2_helper : std_logic_vector(15 downto 0):="0000000000000000"; 
signal mac_inp2 : std_logic_vector(15 downto 0) ;

signal add_bais : std_logic_vector(7 downto 0):="00000000"; 
signal img_into_weight : std_logic_vector(15 downto 0) :="0000000000000000";
signal img_into_weight_plus_bias :std_logic_vector(15 downto 0);
signal we : std_logic := '1' ;
signal re : std_logic := '0' ;
signal we_hl : std_logic := '0' ;
signal re_hl : std_logic := '0' ;
signal we_fl : std_logic := '0' ;
signal re_fl : std_logic := '0' ;
signal rom_addr : std_logic_vector(15 downto 0) := "0000000000000000"; 
signal rom_curr_addr : std_logic_vector(15 downto 0) := "0000000000000000"; 
signal rom_bias_addr : std_logic_vector(15 downto 0) := std_logic_vector(to_unsigned(bais1_base,16)); 
signal ram_addr : std_logic_vector(10 downto 0) :="00000000000" ;
signal shifter_en : std_logic := '1' ; 
signal rom_op : std_logic_vector(7 downto 0)  ;


signal rom_op_conc : std_logic_vector(15 downto 0) ; 

signal ram_op : std_logic_vector(15 downto 0) ; 

signal hl1_ram : std_logic_vector(15 downto 0 );
signal fl1_ram : std_logic_vector(15 downto 0 );
signal relu_op : std_logic_vector(15 downto 0) ; 
signal shifter_op :std_logic_vector(15 downto 0) ; 
signal hidden_layer_1_addr : std_logic_vector(10 downto 0) := "00000000000";
signal final_layer_addr : std_logic_vector(10 downto 0) := "00000000000";
signal final_output : std_logic_vector(3 downto 0) ;
--signal seg : std_logic_vector(6 downto 0) ;

--signal state : std

    --fsm1 
signal state_fsm1 : integer := 0;
signal s0_fsm1 : integer := 0; -- read from rom
signal s1_fsm1 : integer := 1;     -- write in ram from rom 
signal s2_fsm1 : integer := 2;

--fsm2

signal state_fsm2 : integer := 0;
signal s0_fsm2 : integer := 0; 
signal s1_fsm2 : integer := 1;     
signal s2_fsm2 : integer := 2;     
signal s3_fsm2 : integer := 3;                                     
signal s4_fsm2 : integer := 4;  
signal s5_fsm2 : integer := 5;  
signal s6_fsm2 : integer := 6;  
signal s7_fsm2 : integer := 7;  
signal s8_fsm2 : integer := 8;  

--fsm3

signal state_fsm3 : integer := 0;
signal s0_fsm3 : integer := 0; 
signal s1_fsm3 : integer := 1;
signal current : integer := 0;
signal max_ind : integer := 0 ;
signal max_val : std_logic_vector(15 downto 0) := X"0000"; 

begin 

--          clk <= not clk after clk_period / 2 ;
    
          img_read_from_rom  : rom port map(clk , rom_addr , rom_op) ; 
          
          img_write_or_read_in_ram : ram port map(clk ,rom_op_conc ,  ram_addr ,we ,re , ram_op) ;

          relu : comparator port map( img_into_weight_plus_bias , relu_op  ) ; 
          shifter_port : shifter port map(shifter_en , relu_op , shifter_op  );       
          hidden1 : mac port map(clk , rom_op ,mac_inp2 , ctrl, img_into_weight);
          hidden_layer_1: ram port map(clk , shifter_op , hidden_layer_1_addr , we_hl , re_hl , hl1_ram);

          final_layer_1: ram port map(clk , shifter_op , final_layer_addr , we_fl , re_fl , fl1_ram);
          
          decoder : seven_seg_decoder port map(final_output , seg ) ;
        rom_op_conc <= "00000000"&rom_op when rom_op(7) = '0' else 
                        "11111111"& rom_op ; 
        
           mac_inp2 <= ram_op when state_fsm1=s0_fsm1 or state_fsm2 = s1_fsm2 or state_fsm2 = s2_fsm2 or state_fsm2 = s3_fsm2 else 
                       hl1_ram ; 
                       
                     
            final_output <= std_logic_vector(to_unsigned(max_ind - 1 , 4)); 
            an(0)  <= '1';
            an(1)  <= '1';
            an(2)  <= '1';
            an(3)  <= '0';

    process(clk)
    
   
    
    
    constant img_data_size : integer := 784;
    
    begin
        if(rising_edge(clk)) then
        
            if(state_fsm1=s0_fsm1) then
                if(current = img_data_size)then 
                    state_fsm1 <= s2_fsm1 ; 
                    
                
                    if(state_fsm2 = s0_fsm2) then 
                         ram_addr <= "00000000000";
                         rom_addr <= std_logic_vector(to_unsigned(weight1_base , 16)) ; 
                         current <= 0 ;
                         
                        
                        
                         re <= '1' ;  
                        state_fsm2 <= s1_fsm2 ; 
                        
                       
                        
                        
                    end if ; 
                else  
                    we <= '1';
                    rom_addr <= rom_addr + '1';
                    
                    state_fsm1 <= s1_fsm1 ; 
                 end if ; 
            elsif (state_fsm1 = s1_fsm1) then 
             
                 if(current = img_data_size) then
                    state_fsm1 <= s2_fsm1 ;
                 else    
                    current <= current + 1;
                    state_fsm1 <= s2_fsm1;
                    we <= '0'; 
                    ram_addr <= ram_addr + '1' ;    
                    state_fsm1 <= s0_fsm1 ; 
                  
                  end if ;
                 
              
            
            
            end if;
            if(state_fsm2 = s0_fsm2) then       --this is the waiting state of fsm1
            
            elsif (state_fsm2 = s1_fsm2) then   -- read image 
                ram_addr <= ram_addr + '1' ; 
                rom_addr <= rom_addr + '1' ; 
                current <= current + 1 ;
                if(current = 0 ) then 
                    ctrl <=  '1'; 
                else
                    ctrl <= '0' ; 
                end if ;

                if (current = 783) then 
                    state_fsm2 <= s2_fsm2 ;
                    rom_curr_addr <= rom_addr ; 
                    rom_addr <= rom_bias_addr ;
                    
                    
                end if ;
                if(to_integer(unsigned(hidden_layer_1_addr)) = 64) then 
                    state_fsm2 <= s4_fsm2 ; 
                    we_hl <='0';
                    re_hl <= '1';
                    we_fl <= '1' ;
                    re_fl <= '0'; 
                   
                    rom_addr <= std_logic_vector(to_unsigned(weight2_base , 16) ) ;
                    current <= 0 ; 
                    
                    rom_bias_addr <= std_logic_vector(to_unsigned(bais2_base,16)) ; 
                    
                    hidden_layer_1_addr <= "00000000001";
                    
                    
                    
                end if ;
            elsif(state_fsm2 = s2_fsm2) then 
            
                ctrl <= '1' ; 
                ram_addr <= "00000000000"; 
                current <= 0 ; 
                rom_addr <= rom_curr_addr + 1 ; 
                rom_bias_addr <= rom_bias_addr + 1;  
                state_fsm2 <= s3_fsm2 ; 
                we_hl <= '1' ;
                
                
             elsif(state_fsm2 = s3_fsm2) then 
             
                img_into_weight_plus_bias <= img_into_weight + rom_op_conc ; 
                
                hidden_layer_1_addr <= hidden_layer_1_addr +'1'; 
                
                state_fsm2 <= s1_fsm2 ; 
                
             
                --
                
                
                
                -- final layer computation ahead 
                
                
                
             elsif(state_fsm2 = s4_fsm2) then 
             
                hidden_layer_1_addr <= hidden_layer_1_addr + '1'; 
                
                rom_addr <= rom_addr + '1' ; 
                current <= current + 1 ;
                
                if(current = 0 ) then 
                    ctrl <=  '1'; 
                else
                    ctrl <= '0' ; 
                end if ;            
                if (current = 63) then 
                    state_fsm2 <= s5_fsm2 ;
                    rom_curr_addr <= rom_addr ; 
                    rom_addr <= rom_bias_addr ;
                          
                end if; 
                
--            
                if(to_integer(unsigned(final_layer_addr)) = 10) then 
                    state_fsm2 <= s7_fsm2 ;  
                    we_hl <='0';
                    re_hl <= '0';
                    we_fl <= '0';
                    re_fl <= '1'; 
                    final_layer_addr <= "00000000000";
                    
                    
                    end if;
                    
                    --handle 
                    
                    
                 elsif(state_fsm2 = s5_fsm2) then 
                   ctrl <= '1' ; 
                   hidden_layer_1_addr <= "00000000001";
                   current <= 0 ; 
                   rom_addr <= rom_curr_addr + 1 ; 
                   rom_bias_addr <= rom_bias_addr + 1;  
                    state_fsm2 <= s6_fsm2 ; 
                   we_fl <= '1' ;

                elsif(state_fsm2 = s6_fsm2) then 
            
                   img_into_weight_plus_bias <= img_into_weight + rom_op_conc ; 

                   final_layer_addr <= final_layer_addr +'1'; 

                   state_fsm2 <= s4_fsm2 ;     
                   
                   
                 elsif(state_fsm2 = s7_fsm2) then 
                    if(fl1_ram > max_val) then 
                       max_val <= fl1_ram ;
                       max_ind <= to_integer(unsigned(final_layer_addr ))-1;
                    end if ;
                    final_layer_addr <= final_layer_addr + 1; 
                    if(final_layer_addr = 11) then 
                       state_fsm2 <= s8_fsm2 ; 
                    end if ;

                 end if ;
                 
                 
            end if ;
            
   
    
    
--    for weight_col in 0 to 63 loop 
--        ctrl <= '1' ;
--        bias_addr <= std_logic_vector(to_unsigned( bais1_base + weight_col, 16)) ;
--        for  img_row in 0 to 783 loop
           
--            img_addr <=  std_logic_vector(to_unsigned(img_row + img_base,16))  ;
--            weight_addr <= std_logic_vector(to_unsigned( weight1_base  + weight_col*784 + img_row , 16 ))  ;
--            img_into_weight_plus_bias <= img_into_weight ;
--            wait for clk_period ;
--            ctrl <= '0' ;        
--        end loop;
--        ram_addr <= std_logic_vector(to_unsigned( weight_col , 8))  ;
        

--    end loop;

    




    
    
end process ;

end beh ;
           
           
           
           
           