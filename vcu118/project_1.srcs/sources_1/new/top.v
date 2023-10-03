`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/14/2023 06:30:15 PM
// Design Name: 
// Module Name: top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

// 125_000_000 / 125_0 = 100_000
`define SYSCLK_DIV 124

module top(
  /* FPGA input signals */
  input  CPU_RESET,
  input  CLK_125MHZ_P,
  input  CLK_125MHZ_N,
  
  /* FPGA IO signals */
  input  GPIO_SW_C,
  output GPIO_LED0,
  output GPIO_LED1,
  output GPIO_LED2,
  output GPIO_LED7,
  
  input  GPIO_DIP_SW1,
  input  GPIO_DIP_SW2,
  input  GPIO_DIP_SW3,
  input  GPIO_DIP_SW4,
  
  
  // TCK
  input  PMOD0_2_LS,
  // TDI
  input  PMOD0_4_LS,
  // TDO
  output PMOD0_0_LS,
  // TMS
  input  PMOD0_5_LS,
  
  output PMOD1_0_LS,
  output PMOD1_1_LS,
  output PMOD1_2_LS,
  output PMOD1_3_LS,
  output PMOD1_4_LS,
    
  // HSPC is connecting to DUT
  // HPC is the narrower one
  // PMOD0 is J52
  
  /* DUT connections */
  // reset
  output FMC_HPC1_LA06_N,
  // cclk2
  output FMC_HPC1_LA04_P,
  // TCK
  output FMC_HPC1_LA00_CC_P,
  // TDI
  output FMC_HPC1_LA03_P,
  // TDO
  input  FMC_HPC1_LA03_N,
  // TMS
  output FMC_HPC1_LA08_P,  
  // GPIO0
  input  FMC_HPC1_HA05_N,
  // UART RX
  output FMC_HPC1_LA10_P,
  // UART TX
  input  FMC_HPC1_LA10_N
);

  /* FPGA clocking, 125MHz */
  wire clk;
  IBUFGDS bufgd_inst(.I(CLK_125MHZ_P), .IB(CLK_125MHZ_N), .O(clk));

//  wire clk_100;
//  wire clk_50;
//  wire clk_20;
//  wire clk_10;
//  wire clk_1;
  
//  clk_wiz_0 clk_wiz_0(
//    // Clock out ports
//    .clk_out1(clk_100),
//    .clk_out2(clk_50),
//    .clk_out3(clk_20),
//    .clk_out4(clk_10),
//    // Status and control signals
//    .resetn(~CPU_RESET),
//    .locked(),
//    // Clock in ports
//    .clk_in1_p(CLK_125MHZ_P),
//    .clk_in1_n(CLK_125MHZ_N)
//  );
  
  
  /* FPGA IO signals */
  assign GPIO_LED0 = 1'b1;
  assign GPIO_LED1 = 1'b1;
  assign GPIO_LED2 = 1'b0;
  assign GPIO_LED7 = 1'b0;
  
  assign PMOD1_0_LS = counter < (`SYSCLK_DIV / 2);
  assign PMOD1_2_LS = 'b0;
  assign PMOD1_3_LS = 'b0;
  
  /* DUT clock generation */
  reg [31:0] counter;
  always @(posedge clk) begin
    if (counter == `SYSCLK_DIV)
        counter <= 'd0;
    else
        counter <= counter + 1;
  end
  
  /* DUT signals */
  // reset
  assign FMC_HPC1_LA06_N = ~GPIO_SW_C;
  
  // CCLK
  assign FMC_HPC1_LA04_P = counter < (`SYSCLK_DIV / 2);
  
  // JTAG
  // TCK
  assign FMC_HPC1_LA00_CC_P = PMOD0_2_LS;
  // TDI
  assign FMC_HPC1_LA03_P = PMOD0_4_LS;
  // TDO
  assign PMOD0_0_LS = FMC_HPC1_LA03_N;
  // TMS
  assign FMC_HPC1_LA08_P = PMOD0_5_LS;
  
  // GPIO
  // GPIO0 
  assign PMOD1_1_LS = FMC_HPC1_HA05_N;
  
  // UART
  assign FMC_HPC1_LA10_P = 'b1;
  assign PMOD1_4_LS = FMC_HPC1_LA10_N;
  
endmodule
