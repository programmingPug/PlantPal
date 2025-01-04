import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';
import { SoilData } from '../models/soil-data.model';

@Injectable({
  providedIn: 'root'
})
export class SoilDataService {
  private apiUrl = 'http://localhost:5284/api/'; // Replace with your actual API endpoint
  
  constructor(private http: HttpClient) {}

  getPlants(): Observable<SoilData[]> {
    return this.http.get<SoilData[]>(this.apiUrl + 'devices');
  }

    // Update device nickname
    updateNickname(name: string, nickname: string): Observable<any> {
      const url = `${this.apiUrl}devices/${encodeURIComponent(name)}/nickname`;
      return this.http.put(url, { nickname: nickname });
    }

}
