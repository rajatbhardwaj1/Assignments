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

// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                5 Mbps, 2 ms
//
//
// We want to look at changes in the ns-3 TCP congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off
// application is not created until Application Start time, so we wouldn't be
// able to hook the socket (now) at configuration time.  Second, even if we
// could arrange a call after start time, the socket is not public so we
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass
// this socket into the constructor of our simple application which we then
// install in the source node.
// ===========================================================================
//
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
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}



int main (int argc, char *argv[])
{
  std::string tcpVariant = "ns3::TcpVegas";             /* TCP variant type. */



 
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (tcpVariant, &tcpTid), "TypeId " << tcpVariant << " not found");
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    Ptr<Node> n1 = CreateObject<Node>();
    Ptr<Node> n2 = CreateObject<Node>();
    Ptr<Node> n3 = CreateObject<Node>();
    Ptr<Node> n4 = CreateObject<Node>();
    Ptr<Node> n5 = CreateObject<Node>();

    Names::Add("n1", n1);
    Names::Add("n2", n2);
    Names::Add("n3", n3);
    Names::Add("n4", n4);
    Names::Add("n5", n5);

    NodeContainer n1n2(n1, n2);
    NodeContainer n2n3(n2, n3);
    NodeContainer n3n4(n3, n4);
    NodeContainer n3n5(n3, n5);

    NodeContainer global( n1, n2, n3, n4 , n5);

    // create link
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("0.5Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
    NetDeviceContainer d1d2 = p2p.Install(n1n2);
    NetDeviceContainer d2d3 = p2p.Install(n2n3);
    NetDeviceContainer d3d4 = p2p.Install(n3n4);
    NetDeviceContainer d3d5 = p2p.Install(n3n5);
    // create internet stack
    InternetStackHelper internet;
    internet.Install (global);

    Ipv4AddressHelper ipv4;

    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);

    ipv4.SetBase ("10.2.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);

    ipv4.SetBase ("10.3.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i3i4 = ipv4.Assign (d3d4);

    ipv4.SetBase ("10.4.4.0", "255.255.255.0");
    Ipv4InterfaceContainer i3i5 = ipv4.Assign (d3d5);


    Config::SetDefault("ns3::Ipv4GlobalRouting::RandomEcmpRouting",     BooleanValue(true)); // enable multi-path routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    // install first upd application from 20 to 30 sec
    UdpEchoServerHelper echoServer1(9999);
    ApplicationContainer serverApps1 = echoServer1.Install (n5);
    serverApps1.Start (Seconds (20.0));
    serverApps1.Stop (Seconds (30.0));


    UdpEchoClientHelper echoClient1(i1i2.GetAddress (1), 9999);
    echoClient1.SetAttribute ("MaxPackets", UintegerValue (100000));
    echoClient1.SetAttribute ("Interval", TimeValue (Seconds (0.00416*8)));
    echoClient1.SetAttribute ("PacketSize", UintegerValue (1040));
    ApplicationContainer clientApps1 = echoClient1.Install (n1);
    clientApps1.Start (Seconds (20.0));
    clientApps1.Stop (Seconds (30.0));



//declaring 2nd udp application

    UdpEchoServerHelper echoServer2(9999);
    ApplicationContainer serverApps2 = echoServer2.Install (n5);
    serverApps2.Start (Seconds (30.0));
    serverApps2.Stop (Seconds (100.0));


    UdpEchoClientHelper echoClient2(i1i2.GetAddress (1), 9999);
    echoClient2.SetAttribute ("MaxPackets", UintegerValue (100000));
    echoClient2.SetAttribute ("Interval", TimeValue (Seconds (0.00416*4)));
    echoClient2.SetAttribute ("PacketSize", UintegerValue (1040));
    ApplicationContainer clientApps2 = echoClient2.Install (n1);
    clientApps2.Start (Seconds (30.0));
    clientApps2.Stop (Seconds (100.0));

  p2p.EnablePcapAll ("test");
  



//declaring tcp app 



  uint16_t sinkPort = 8080;
  Address sinkAddress;
  Address anyAddress;
  std::string probeType;
  std::string tracePath;
  


  sinkAddress = InetSocketAddress (i3i4.GetAddress(1), sinkPort);
  anyAddress = InetSocketAddress (Ipv4Address::GetAny (), sinkPort);
  probeType = "ns3::Ipv4PacketProbe";
  tracePath = "/NodeList/*/$ns3::Ipv4L3Protocol/Tx";
    
 

  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", sinkAddress);

  //client
  ApplicationContainer sinkApps = packetSinkHelper.Install(n3n4.Get(1));
  sinkApps.Start (Seconds (1.));
  sinkApps.Stop (Seconds (100));

  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (n1n2.Get (0), TcpSocketFactory::GetTypeId ());

  Ptr<MyApp> app = CreateObject<MyApp> ();
  //tcp 
  app->Setup (ns3TcpSocket, sinkAddress, 2000, 10000, DataRate ("0.25Mbps"));
  n1n2.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (100.));



  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("task3.cwnd");
  ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));

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
  anim.SetConstantPosition(n1n2.Get(0) ,1.0 , 75.0);
  anim.SetConstantPosition(n2n3.Get(0) ,50.0 , 75.0);
  anim.SetConstantPosition(n3n4.Get(0) ,100.0 , 75.0);
  anim.SetConstantPosition(n3n4.Get(1) ,150.0 , 50.0);
  anim.SetConstantPosition(n3n5.Get(1) ,150.0 , 100.0);
  // anim.SetConstantPosition(n0n1.Get(1) ,3.0 , 2.0);
  // anim.SetConstantPosition(n1n2.Get(1) ,4.0 , 2.0);
  // anim.SetConstantPosition(n2n4.Get(1) ,5.0 , 2.0);
  // anim.SetConstantPosition(n3n4.Get(0) ,6.0 , 2.0);
  Simulator::Run ();
  std::cout<<i1i2.GetAddress(1)<<" "<<i1i2.GetAddress(0) <<'\n';
  Simulator::Destroy ();

  return 0;
}

