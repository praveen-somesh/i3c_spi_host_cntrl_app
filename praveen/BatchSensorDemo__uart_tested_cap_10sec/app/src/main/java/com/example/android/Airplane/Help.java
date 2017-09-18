package com.example.android.Airplane;

import android.app.ActionBar;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.app.Activity;
import android.view.View;
import android.widget.Button;

public class Help extends Activity {

    private Button button_exit;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_help);

        button_exit=(Button)findViewById(R.id.exit);
        button_exit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
        ActionBar actionBar;
        actionBar = getActionBar();
        ColorDrawable colorDrawable = new ColorDrawable(Color.parseColor("#0080ff"));
        actionBar.setBackgroundDrawable(colorDrawable);
    }

}
