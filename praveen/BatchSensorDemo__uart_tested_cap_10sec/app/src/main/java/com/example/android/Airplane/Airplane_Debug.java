package com.example.android.Airplane;

import android.app.ActionBar;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;


public class Airplane_Debug extends Activity {
    private final static  String TAG="AirplaneDebug";
    static String debug_log="";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity__debug);
        Log.e(TAG,"enter Airplane Debug Activity");
        final EditText edit_addr_h=(EditText)findViewById(R.id.addr_h);
        final EditText edit_addr_l=(EditText)findViewById(R.id.addr_l);
        final EditText edit_value=(EditText)findViewById(R.id.addr_value);

        ActionBar actionBar;
        actionBar = getActionBar();
        ColorDrawable colorDrawable = new ColorDrawable(Color.parseColor("#0080ff"));
        actionBar.setBackgroundDrawable(colorDrawable);

        final TextView edit_log=(TextView)findViewById(R.id.demo_debug_log);

        Button button_debug_read=(Button)findViewById(R.id.demo_read);
        Button button_debug_write=(Button)findViewById(R.id.demo_write);
        Button button_debug_clear=(Button)findViewById(R.id.demo_clear_log);

        Button button_debug_finish=(Button)findViewById(R.id.demo_debug_finish);

        final Intent intent=new Intent(this,Android_Airplane_Activity.class);
        button_debug_finish.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.e(TAG,"button_finish pressed");
                finish();
//startActivity(intent);
            }
        });
        button_debug_write.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.e(TAG,"debug data input before");

                String str=edit_addr_h.getText().toString();
                byte addr_h=(byte)  (Integer.parseInt(str,16)   & 0xFF);

                str=edit_addr_l.getText().toString();
                byte addr_l=(byte)  (Integer.parseInt(str,16)   & 0xFF);

                str=edit_value.getText().toString();
                byte value=(byte)  (Integer.parseInt(str,16)   & 0xFF);

                Log.e(TAG, "debug data input successfully");


                Android_Airplane_Activity.reg_wr_byte(addr_h, addr_l , value);
                debug_log=debug_log +"write_reg "+String.format("%02x" ,addr_h)+String.format("%02x" ,addr_l)+"value :"+String.format("02x ",value)+"\n";

                edit_log.setText(debug_log);

            }
        });
        button_debug_read.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                BluetoothLeService.data_resp = 0;
                String str= edit_addr_h.getText().toString();
                byte addr_h =(byte) (Integer.parseInt(str,16) & 0xFF);

                str= edit_addr_l.getText().toString();
                byte addr_l =(byte) (Integer.parseInt(str,16) & 0xFF);

                str= edit_value.getText().toString();
                byte value =(byte) (Integer.parseInt(str,16) & 0xFF);


                Android_Airplane_Activity.reg_rd(1,addr_h, addr_l);


                while(BluetoothLeService.data_resp == 0) {
                }

                BluetoothLeService.data_resp = 0;
                debug_log = debug_log + "read_reg "+String.format("%02x", addr_h)+String.format("%02x ", addr_l)+"value: "
                        +String.format("%02x ", BluetoothLeService.read_value)+"\n";

                edit_log.setText(debug_log);
            }
        });

        button_debug_clear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                debug_log="";
                edit_log.setText(debug_log);
            }
        });


    }

    private  void save2file(String data, String filename){
        int num= 0;


        Log.e(TAG,"save2file the data is "+data);
        String [] data_element=data.split(" ");


        for (num=0; num<data_element.length; num++){
            if(num%2==1);
            //	datafile.saveToSDCard(filename, data_element[num]);
        }
    }
}
