import React from "react";
import Webimage1 from "./Rectangle4.png";
import Create from "./Checkout.png";

import "./check.css";

function Check() {
  return (
    <div id="sup">
      <div>
        <span>
          <img src={Webimage1} alt="img1"></img>
        </span>
        <span>
          <div id="cont">
            <img src={Create} alt="create"></img>
            <h2>When you done check out and get it delivered.</h2>
            <p>When you done check out and get it delivered with ease.</p>
          </div>
        </span>
      </div>
    </div>
  );
}
export default Check;
