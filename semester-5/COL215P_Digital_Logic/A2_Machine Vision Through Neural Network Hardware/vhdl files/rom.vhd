----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Geetansh and Rajat
-- 
-- Create Date: 09.09.2022 13:39:20
-- Design Name: 
-- Module Name: mac - Behavioral
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
use IEEE.STD_LOGIC_UNSIGNED.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;
use std.textio.all;
-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity rom is
generic (
    ADDR_WIDTH : integer := 16;
    DATA_WIDTH : integer := 8;
    IMAGE_SIZE : integer := 784;
    TOTAL_ROM_SIZE : integer := 65536  ;
    WEIGHT_BAIS_SIZE : integer := 50890;
    IMAGE_FILE_NAME : string :="imgdata_digit7.mif";
    WEIGHT_BIAS_FILE_NAME : string := "weights_bias.mif"
);
port(
    clk : in std_logic; 
    addr : in std_logic_vector(15 downto 0); 
    dout : out std_logic_vector(7 downto  0) 
    
);


end rom;
architecture Behavioral of rom is
    TYPE mem_type IS ARRAY(0 TO TOTAL_ROM_SIZE) OF std_logic_vector((DATA_WIDTH-1) DOWNTO 0);
    impure function init_mem(img_file_name : in string ; wb_file_name :in string) return mem_type is
    file img_file : text open read_mode is img_file_name;
    file weight_bias_file : text open read_mode is weight_bias_file_name;

    variable mif_line : line;
    variable temp_bv : bit_vector(DATA_WIDTH-1 downto 0);
    variable temp_mem : mem_type;
    begin
        for i in mem_type'range loop
            if i < 784 then 
                readline(img_file, mif_line);
                read(mif_line, temp_bv);
                temp_mem(i) := to_stdlogicvector(temp_bv);
            end if ;
            if i >= 1024 and i < 51914  then
                readline(weight_bias_file, mif_line);
                read(mif_line, temp_bv);            
                temp_mem(i) := to_stdlogicvector(temp_bv);
            end if ;
        end loop;
             
            return temp_mem;
        end function;
        -- Signal declarations
        signal rom_block: mem_type := init_mem(IMAGE_FILE_NAME , WEIGHT_BIAS_FILE_NAME);
        begin
        
        process(clk)

            begin 

                if(rising_edge(clk)) then 

                dout <= rom_block(to_integer(unsigned(addr)));

            end if ;

        end process ;


  
end Behavioral;























