import React from "react";
import Webimage1 from "./Rectangle2.png";
import Create from "./create.png";

import "./cent.css";

function Cent() {
  return (
    <div>
      <hr id="line"></hr>
      <div>
        <h1 id="hd">How the app works</h1>
      </div>
      <div>
        <span>
          <img src={Webimage1} alt="img1"></img>
        </span>
        <span>
          <div id="cont">
            <img src={Create} alt="create"></img>
            <h2>Create/login to an existing account to get started</h2>
            <p>An account is created with your email and a desired password</p>
          </div>
        </span>
      </div>
    </div>
  );
}
export default Cent;
