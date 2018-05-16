<?php
$servername = "localhost";
$db="id5275176_cyclelock";
$username = "id5275176_linkitone";
$password = "linkitone";

// Create connection
$conn = mysqli_connect($servername, $username,$password,$db);

// Check connection
if (!$conn) {
    die("Connection failed: " . mysqli_connect_error());
}
//echo "Connected successfully";

    ///Parser
	define("LOCK_HEADER","CMDR");
	define("SERVER_HEADER","CMDS");
	define("Reserve data",0);

function check_geofenced($lat,$lng)
{
	$query="SELECT * FROM Docks";
	$result=mysqli_query($conn,$query);
	
	$dock1lat= 2832.71;
    $dock1long= 7711.6;
	//while($row = mysqli_fetch_assoc($result)) {
        //$dock1lat=$row["latitude"];
        //$dock1long=$row["longitude"];
	//$dock1lat= 2832.7065;
    //$dock1long= 7711.5954;
	 $lati= (10000*$lat);
     $longi= (10000*$lng);
     $dock1lati= 10000*$dock1lat;
     $dock1longi= 10000*$dock1long;
	if(((($dock1lati-100)<=$lati)&&($lati<($dock1lati+100)))&&((($dock1longi-100)<=$longi)&&($longi<($dock1longi+100))))
        return 1;
	//}
    else
        return 0;
}	
	
		//If SIGN Up
	if(isset($_GET["signup"]))
	{
	    
		$id=$_GET["rfidsignup"];
		$entry=$_GET["usernamesignup"];
		$bal=100;
		$pass=$_GET["passwordsignup"];
		$em_id=$_GET["emailsignup"];
	
		$query="INSERT INTO User (user_id,entry_no,balance,email_id,password) VALUES ('$id','$entry','$bal','$em_id','$pass')";
	
	mysqli_query($conn, $query);
   
	
		header("Location:../index1.php");
		
	}
	
	//if LOGIN
	if(isset($_GET["login"]))
	{   
	    $a=1;
	    
		$entry=$_GET["username"];
		$crypt_password=$_GET["password"];
		$query="SELECT * FROM User where entry_no='$entry' AND password='$crypt_password'";
		$result=mysqli_query($conn,$query);
		//if(!$result)
		//echo "error".mysqli_error($conn);
		
		if (mysqli_num_rows($result) > 0) {
		    $row=mysqli_fetch_assoc($result);
		    $bal=$row["balance"];
		    $a=0;
    
		}
		/*
		if($a==1)
			{
			   echo "<script>
        alert('Invalid Id/Password');
        window.location.href='/';
        </script>";
     }*/
		
	}
	
	if(isset($_GET["command"]))
	{
	$cmd_string=$_GET["command"];
	$cmd_array=explode(',',$cmd_string);
	$len_data=count($cmd_array);
	
	
	//Stores the first 5 data -STX,CODE,IMEI,TIME,CMD
	$data=array();
	$response_arr=array();
	for($i=0;$i<5;$i++)
	{
	    $data[$i]=$cmd_array[$i];
	}
	
	//When user shows RFID first time R0
	if($data[4]=="R0")
	{
		//get the rest of data
	    for($i=5;$i<8;$i++)
	    $data[$i]=$cmd_array[$i];
		$uid=$data[6];		//get the user id of the lock
		$query="SELECT * FROM User where user_id= '".$uid."'";
		$result=mysqli_query($conn,$query);
		
		if (mysqli_num_rows($result) > 0) {
		    $j=0;
			$response_arr[$j++]=constant("SERVER_HEADER");
			$response_arr[$j++]=$data[1];       //copy device type
			$response_arr[$j++]=$data[2];       //copy imei number of device
			$response_arr[$j++]=date("Ymd").date("his");
			$response_arr[$j++]="L0";		//command code
			$response_arr[$j++]= 0 ;     		//Unlock
			$response_arr[$j++]= $uid;     		
			$response_arr[$j++]= $data[7];		//TS    		
			$response_string="";
			foreach($response_arr as $r)
			$response_string.=$r.",";
			
			$response_string.="#<LF>";
			echo "Server's response:".$response_string;
		}
		
	}
	
	//command is unlock
	else if($data[4]=="L0")
	{
	    
		//get the rest 	of data
		//get the rest of data
	    for($i=5;$i<8;$i++)
	    $data[$i]=$cmd_array[$i];
		$uid=$data[6];		//get the user id of the lock
	    for($i=5;$i<8;$i++)
	    $data[$i]=$cmd_array[$i];
	    $query="SELECT * FROM Lock_info where imei= ".$data[2];
		$result=mysqli_query($conn,$query);
		if (mysqli_num_rows($result) > 0){
		  
		    while($row = mysqli_fetch_assoc($result)){
		        if($row["ride"]==0)//check if this is the unlock signal sent for the first time the user starts riding
		        {
		            $r_id=$row["id"];
		            $r_did=$row["d_id"];
		            $t=time();
		            $query="INSERT INTO `Ride`(`user_id`, `lock_id`, `start_point`, `start_time`, `riding_status`) VALUES ('$uid','$r_id','$r_did','$t','1')";
		            $res=mysqli_query($conn, $query);
					/*if(!$res)
						echo "Insert record error".mysqli_error($conn);
						else
						echo "Recod inserted";*/
		            $query="UPDATE Lock_info SET ride=1,status=0 WHERE imei= ".$data[2];
		            mysqli_query($conn,$query);
		            $query="SELECT * FROM Docks where id= ".$row['d_id'];
		            $result=mysqli_query($conn,$query);
		            if (mysqli_num_rows($result) > 0){
		            $row = mysqli_fetch_assoc($result);
		            
		            $query="UPDATE Docks SET no_of_cycle=no_of_cycle-1 WHERE imei= ".$data[2];
		            mysqli_query($conn,$query);
		            }
		            
		        }
		    }
		    
		}
		
		
	}
	
	//if the command is lock
	if($data[4]=="L1")
	{
	    
	    //get the rest of data
	    //for($i=5;$i<8;$i++)
	    //$data[$i]=$cmd_array[$i];
	   
	    $data[5]=$cmd_array[5];
	    //DO SOMETHING
		$uid=$data[5];		//get the user id of the lock
		
		$query="SELECT * FROM Lock_info where imei= ".$data[2];
		$result=mysqli_query($conn,$query);
		if (mysqli_num_rows($result) > 0){
		$row = mysqli_fetch_assoc($result);
		$r_lat=$row["latitude"];
		$r_lng=$row["longitude"];
		if(check_geofenced($r_lat,$r_lng)==1)
		{
			//get the corresponding ride
			$query="SELECT * FROM Ride where lock_id= ".$row["id"];
		$result=mysqli_query($conn,$query);
		$ride_row = mysqli_fetch_assoc($result);
		if(row["ride"]==1 && $ride_row["riding_status"]==1)
		{
			$d=time()-$ride_row["start_time"];
			$query="UPDATE Ride SET duration='".$d."' WHERE lock_id= ".$row["id"];
		            mysqli_query($conn,$query);
			$charges=$d*0.5;
			$query="UPDATE Lock_info SET ride=0,status=1 WHERE imei= ".$data[2];
		            mysqli_query($conn,$query);
				
			$query="UPDATE User SET balance='".balance-$charges."' WHERE user_id= ".$ride_row["user_id"];
		            mysqli_query($conn,$query);
			$query="DELETE Ride where user_id='".$ride_row["user_id"]."' and lock_id= ".$row["id"];
			mysqli_query($conn,$query);
		}
			
			$j=0;
	    $response_arr[$j++]=constant("SERVER_HEADER");
	    $response_arr[$j++]=$data[1];       //copy device type
	    $response_arr[$j++]=$data[2];       //copy imei number of device
	    $response_arr[$j++]=date("Ymd").date("his");
	    $response_arr[$j++]="Re";
	    $response_arr[$j++]="L1" ;       //command code
		$response_string="";
		foreach($response_arr as $r)
		$response_string.=$r.",";
		
		
		$response_string.="#<LF>";
		echo "Server's response:".$response_string;
		}
		else		//if not in geofenced area send unlock command
		{
		    //echo "NOt in geofenced";
			$j=0;
	    $response_arr[$j++]=constant("SERVER_HEADER");
	    $response_arr[$j++]=$data[1];       //copy device type
	    $response_arr[$j++]=$data[2];       //copy imei number of device
	    $response_arr[$j++]=date("Ymd").date("his");
	    
	    $response_arr[$j++]="L0" ;       //command code
		$response_string="";
		foreach($response_arr as $r)
		$response_string.=$r.",";
		
		
		$response_string.="#<LF>";
		echo "Server's response:".$response_string;
		}
			
		}
		
	    
	}
	
	
	/*
	//Heartbeat command
	else if($data[4]=="H0")
	{
		//get the rest of data
	    for($i=5;$i<8;$i++)
	    $data[$i]=$cmd_array[$i];
		foreach($lock_obj as $l_o)
		{
			if($l_o->imei==$data[2])
			{
				$l_o->status=$data[5];
				$l_o->battery_level=$data[6];
			}
		}
	
	}
	
	*/
	//Gps info
	else if($data[4]=="D0")
	{
		//get the rest of data
	    
	    $data[5]=$cmd_array[5];		//reserve data
	    $data[6]=$cmd_array[6];		//UTC time from gps
	    $data[7]=$cmd_array[7];		//lat
	    $data[8]=$cmd_array[8];		//lng
		$query="UPDATE Lock_info SET latitude='".$data[7]."',longitude= '".$data[8]."' where imei= ".$data[2];
		$result=mysqli_query($conn,$query);
		//if(!$result)
		//echo "error in update".mysqli_error($conn);
		
		
		//prepare server response
	    $j=0;
	    $response_arr[$j++]=constant("SERVER_HEADER");
	    $response_arr[$j++]=$data[1];       //copy device type
	    $response_arr[$j++]=$data[2];       //copy imei number of device
	    $response_arr[$j++]=date("Ymd").date("his");
	    $response_arr[$j++]="Re";
	    $response_arr[$j++]="D0" ;       //command code
		$response_string="";
		foreach($response_arr as $r)
		$response_string.=$r.",";
		
		$response_string.="#<LF>";
		echo "Server's response to D0 command:".$response_string;
	
	}
	
	
	/*
	///SIGN IN COMMAND
	else if($data[4]=="Q0")
	{
		//get the rest of data
		$data[5]=$cmd_array[5];		//battery
	    
		//create a new lock object
		$new_lock=new Lock(3,$data[2],0,12346.5,215456.545,0,$data[5],0,1);
		array_push($lock_obj,$new_lock);
	
	}
	
	////reply of lock status to server
	else if($data[4]=="S5")
	{
		//get the rest of data
	    for($i=5;$i<10;$i++)
	    $data[$i]=$cmd_array[$i];
		foreach($lock_obj as $l_o)
		{
			if($l_o->imei==$data[2])
			{
				$l_o->battery_level=$data[5];
				$l_o->status=$data[8];
				
				//prepare server response
	    $j=0;
	    $response_arr[$j++]=constant("SERVER_HEADER");
	    $response_arr[$j++]=$data[1];       //copy device type
	    $response_arr[$j++]=$data[2];       //copy imei number of device
	    $response_arr[$j++]=date("Ymd").date("his");
	    $response_arr[$j++]="Re";
	    $response_arr[$j++]="S5" ;       //command code
		$response_string="";
		foreach($response_arr as $r)
		$response_string.=$r.",";
		
		
		$response_string.="#<LF>";
		echo "Server's response:".$response_string;
			}
		}
	
	}
	
	*/
	//Halt
	else if($data[4]=="L2")
	{
		//get the rest of data
	    for($i=5;$i<7;$i++)
	    $data[$i]=$cmd_array[$i];
	
	$query="SELECT * FROM Lock_info where imei= ".$data[2];
		$result=mysqli_query($conn,$query);
		if (mysqli_num_rows($result) > 0){
			$row = mysqli_fetch_assoc($result);
			$query="UPDATE Ride SET riding_status=0 WHERE lock_id= ".$row["id"];
		            mysqli_query($conn,$query);
							//prepare server response
	    $j=0;
	    $response_arr[$j++]=constant("SERVER_HEADER");
	    $response_arr[$j++]=$data[1];       //copy device type
	    $response_arr[$j++]=$data[2];       //copy imei number of device
	    $response_arr[$j++]=date("Ymd").date("his");
	    $response_arr[$j++]="Re";
	    $response_arr[$j++]="L2" ;       //command code
		$response_string="";
		foreach($response_arr as $r)
		$response_string.=$r.",";
		
		
		$response_string.="#<LF>";
		echo "Server's response:".$response_string;
		}
		
	
	}
	
	
	}
?>
<!--
<!DOCTYPE html>
<html>
<head>
	<title>Smart Bicycle App</title>
</head>
<body>
<form action="connect.php" method="GET">
<input type="text" name="command">
</form>

</body>

</html>	
-->

<!DOCTYPE html>
<html>
<head>
	<title>Smart Bicycle App</title>
	<link rel="stylesheet" type="text/css" href="mypage.css">
</head>
<body>
<div class="contain">
<div class="hi"><p>Hi  <?php echo $entry;?></p></div>
<div class="balance"><p class="bal">Your Balance is <?php echo $bal; ?> Rs.</p></div>
	
</div>
<!--
<form action="connect.php" method="GET">
<input type="text" name="command">
</form>
-->
</body>

</html>		


