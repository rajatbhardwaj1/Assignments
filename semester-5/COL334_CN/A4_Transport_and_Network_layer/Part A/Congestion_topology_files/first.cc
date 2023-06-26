/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "ns3/netanim-module.h"
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("task3ScriptExample");

class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  if (InetSocketAddress::IsMatchingType (m_peer))
    {
      m_socket->Bind ();
    }
  else
    {
      m_socket->Bind6 ();
    }
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  // NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}



int main (int argc, char *argv[])
{
  std::string tcpVariant = "ns3::TcpNewRenoPlus";             /* TCP variant type. */



 
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (tcpVariant, &tcpTid), "TypeId " << tcpVariant << " not found");
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    Ptr<Node> n1 = CreateObject<Node>();
    Ptr<Node> n2 = CreateObject<Node>();
    Ptr<Node> n3 = CreateObject<Node>();

    Names::Add("n1", n1);
    Names::Add("n2", n2);
    Names::Add("n3", n3);

    NodeContainer n1n3(n1, n3);
    NodeContainer n2n3(n2, n3);

    NodeContainer global( n1, n2, n3);

    // create link
    PointToPointHelper p2p , p2p1;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("3ms"));


    p2p1.SetDeviceAttribute ("DataRate", StringValue ("9Mbps"));
    p2p1.SetChannelAttribute ("Delay", StringValue ("3ms"));


    NetDeviceContainer d1d3 = p2p.Install(n1n3);
    NetDeviceContainer d2d3 = p2p1.Install(n2n3);
    // create internet stack

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  d1d3.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  d2d3.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

    InternetStackHelper internet;
    internet.Install (global);

    Ipv4AddressHelper ipv4;

    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i3 = ipv4.Assign (d1d3);

    ipv4.SetBase ("10.2.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);

  // p2p.EnablePcapAll ("test");
  



//declaring tcp app 


  uint16_t sinkPort1 = 8080;
  uint16_t sinkPort2 = 8081;
  uint16_t sinkPort3 = 8082;
  Address sinkAddress1 , sinkAddress2, sinkAddress3;
  std::string probeType;
  std::string tracePath;
  


  sinkAddress1 = InetSocketAddress (i1i3.GetAddress(1), sinkPort1);
  sinkAddress2 = InetSocketAddress (i1i3.GetAddress(1), sinkPort2);
  sinkAddress3 = InetSocketAddress (i2i3.GetAddress(1), sinkPort3);
  probeType = "ns3::Ipv4PacketProbe";
  tracePath = "/NodeList/*/$ns3::Ipv4L3Protocol/Tx";
    
 

  PacketSinkHelper packetSinkHelper1 ("ns3::TcpSocketFactory", sinkAddress1);
  PacketSinkHelper packetSinkHelper2 ("ns3::TcpSocketFactory", sinkAddress2);
  PacketSinkHelper packetSinkHelper3 ("ns3::TcpSocketFactory", sinkAddress3);

  //client

  //app sink 1 
  ApplicationContainer sinkApps1 = packetSinkHelper1.Install(n1n3.Get(1));
  sinkApps1.Start (Seconds (1.));
  sinkApps1.Stop (Seconds (20));


  //app sink 2
  ApplicationContainer sinkApps2 = packetSinkHelper2.Install(n1n3.Get(1));
  sinkApps2.Start (Seconds (5.));
  sinkApps2.Stop (Seconds (25));


  //app sink 3
  ApplicationContainer sinkApps3 = packetSinkHelper3.Install(n2n3.Get(1));
  sinkApps3.Start (Seconds (15.));
  sinkApps3.Stop (Seconds (30));


// app source 1 

  Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket (n1n3.Get (0), TcpSocketFactory::GetTypeId ());
  Ptr<Socket> ns3TcpSocket2 = Socket::CreateSocket (n1n3.Get (0), TcpSocketFactory::GetTypeId ());
  Ptr<Socket> ns3TcpSocket3 = Socket::CreateSocket (n2n3.Get (0), TcpSocketFactory::GetTypeId ());

  Ptr<MyApp> app1 = CreateObject<MyApp> ();
  //tcp 
  app1->Setup (ns3TcpSocket1, sinkAddress1, 3000, 10000, DataRate ("1.5Mbps"));
  n1n3.Get (0)->AddApplication (app1);
  app1->SetStartTime (Seconds (1.));
  app1->SetStopTime (Seconds (20.));



//app source 2 


  Ptr<MyApp> app2 = CreateObject<MyApp> ();
  //tcp 
  app2->Setup (ns3TcpSocket2, sinkAddress2, 3000, 10000, DataRate ("1.5Mbps"));
  n1n3.Get (0)->AddApplication (app2);
  app2->SetStartTime (Seconds (5.));
  app2->SetStopTime (Seconds (25.));


//app source 3

  Ptr<MyApp> app3 = CreateObject<MyApp> ();
  //tcp 
  app3->Setup (ns3TcpSocket3, sinkAddress3, 3000, 10000, DataRate ("1.5Mbps"));
  n2n3.Get (0)->AddApplication (app3);
  app3->SetStartTime (Seconds (15.));
  app3->SetStopTime (Seconds (30.));



  AsciiTraceHelper asciiTraceHelper1;
  Ptr<OutputStreamWrapper> stream1= asciiTraceHelper1.CreateFileStream ("connection1.cwnd");
  ns3TcpSocket1->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream1));
  AsciiTraceHelper asciiTraceHelper2;
  Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper2.CreateFileStream ("connection2.cwnd");
  ns3TcpSocket2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream2));
  AsciiTraceHelper asciiTraceHelper3;
  Ptr<OutputStreamWrapper> stream3 = asciiTraceHelper2.CreateFileStream ("connection3.cwnd");
  ns3TcpSocket3->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream3));
  

  PcapHelper pcapHelper;
  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile ("task3.pcap", std::ios::out, PcapHelper::DLT_PPP);

  // Use GnuplotHelper to plot the packet byte count over time
  GnuplotHelper plotHelper;

  // Configure the plot.  The first argument is the file name prefix
  // for the output files generated.  The second, third, and fourth
  // arguments are, respectively, the plot title, x-axis, and y-axis labels
  plotHelper.ConfigurePlot ("task3-packet-byte-count",
                            "Packet Byte Count vs. Time",
                            "Time (Seconds)",
                            "Packet Byte Count");

  // Specify the probe type, trace source path (in configuration namespace), and
  // probe output trace source ("OutputBytes") to plot.  The fourth argument
  // specifies the name of the data series label on the plot.  The last
  // argument formats the plot by specifying where the key should be placed.
  plotHelper.PlotProbe (probeType,
                        tracePath,
                        "OutputBytes",
                        "Packet Byte Count",
                        GnuplotAggregator::KEY_BELOW);

  // Use FileHelper to write out the packet byte count over time
  FileHelper fileHelper;

  // Configure the file to be written, and the formatting of output data.
  fileHelper.ConfigureFile ("task3-packet-byte-count",
                            FileAggregator::FORMATTED);

  //Set the labels for this formatted output file.
  fileHelper.Set2dFormat ("Time (Seconds) = %.3e\tPacket Byte Count = %.0f");

  // Specify the probe type, trace source path (in configuration namespace), and
  // probe output trace source ("OutputBytes") to write.
  fileHelper.WriteProbe (probeType,
                         tracePath,
                         "OutputBytes");

  Simulator::Stop (Seconds (100));


  AnimationInterface  anim("animation.xml");
  anim.SetConstantPosition(n1n3.Get(0) ,1.0 , 20.0);
  anim.SetConstantPosition(n1n3.Get(1) ,100.0 , 50.0);
  anim.SetConstantPosition(n2n3.Get(0) ,1.0 , 100.0);
  // anim.SetConstantPosition(n0n1.Get(1) ,3.0 , 2.0);
  // anim.SetConstantPosition(n1n2.Get(1) ,4.0 , 2.0);
  // anim.SetConstantPosition(n2n4.Get(1) ,5.0 , 2.0);
  // anim.SetConstantPosition(n3n4.Get(0) ,6.0 , 2.0);
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

