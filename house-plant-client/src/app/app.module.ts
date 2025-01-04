import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { MoistureChartComponent } from './features/moisture-chart/moisture-chart.component';

import { BaseChartDirective  } from 'ng2-charts';
import { provideHttpClient } from '@angular/common/http';

@NgModule({
  declarations: [
    AppComponent,
    MoistureChartComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    BaseChartDirective 
  ],
  providers: [provideHttpClient()],
  bootstrap: [AppComponent]
})
export class AppModule { }
