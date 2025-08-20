import React, { useEffect, useState } from "react";
import { useWeb3 } from "../contexts/Web3Context";
import Loading from "./Loading";
import { mapTxError } from "../utils/errors";
import { sendSafeTx } from "../utils/tx";
import { verifyBiometric, getBiometricConfig } from "../utils/biometricService";

export default function VoterPanel() {
  const { web3, account, contract, isConnected, isLoading: web3Loading, error: web3Error, currentElection } = useWeb3();
  const [voterId, setVoterId] = useState("");
  const [candidate, setCandidate] = useState("");
  const [status, setStatus] = useState("");
  const [loading, setLoading] = useState(false);
  const [candidates, setCandidates] = useState([]);
  const [biometricChecked, setBiometricChecked] = useState(false);
  const [biometricStatus, setBiometricStatus] = useState("");
  const [showBiometricModal, setShowBiometricModal] = useState(false);

  useEffect(() => {
    const fetchCandidates = async () => {
      if (contract && currentElection && Number(currentElection.id) > 0) {
        try {
          const list = await contract.methods.getElectionCandidates(Number(currentElection.id)).call();
          setCandidates(list);
        } catch (err) {
          setStatus("❌ Failed to fetch candidates.");
        }
      } else {
        setCandidates([]);
      }
    };

    if (isConnected && contract && currentElection) {
      fetchCandidates();
    } else if (web3Error) {
      setStatus(web3Error);
    }
  }, [contract, isConnected, web3Error, currentElection]);

  const checkRegistration = async () => {
    if (!voterId || voterId.trim().length < 3 || !contract || !currentElection) {
      return setStatus("⚠️ Voter ID is required (min 3 chars) and active election needed.");
    }
    setLoading(true);
    const hash = web3.utils.keccak256(voterId);
    try {
      const isRegistered = await contract.methods.isVoterRegistered(Number(currentElection.id), hash).call();
      setStatus(isRegistered ? "✅ Voter is registered." : "❌ Voter is NOT registered.");
    } catch (err) {
      setStatus("❌ Error checking registration.");
    }
    setLoading(false);
  };

  const checkVoteStatus = async () => {
    if (!voterId || voterId.trim().length < 3 || !contract || !currentElection) {
      return setStatus("⚠️ Voter ID is required (min 3 chars) and active election needed.");
    }
    setLoading(true);
    const hash = web3.utils.keccak256(voterId);
    try {
      const voted = await contract.methods.hasVoterVoted(Number(currentElection.id), hash).call();
      setStatus(voted ? "✅ You have already voted." : "❌ You have not voted yet.");
    } catch (err) {
      setStatus("❌ Error checking vote status.");
    }
    setLoading(false);
  };

  const castVote = async () => {
    if (!voterId || voterId.trim().length < 3 || !candidate || candidate.trim().length < 2 || !contract || !account || !currentElection) {
      return setStatus("⚠️ Voter ID (min 3 chars), candidate name (min 2 chars), and active election are required.");
    }
    
    // Mandatory biometric verification via external service
    try {
      setStatus("🔐 Contacting biometric device...");
      const cfg = getBiometricConfig();
      const result = await verifyBiometric(voterId, { timeoutMs: 45000 });
      if (!result.success) {
        return setStatus("❌ Biometric verification failed. Access denied.");
      }
      setBiometricChecked(true);
      setBiometricStatus("✅ Biometric check passed.");
    } catch (err) {
      return setStatus(`❌ Biometric check error: ${err.message}`);
    }
    
    setLoading(true);
    const hash = web3.utils.keccak256(voterId);
    
    try {
      // Use improved transaction utility with retry logic
      const receipt = await sendSafeTx(
        web3, 
        contract.methods.vote(Number(currentElection.id), hash, candidate), 
        account,
        {
          maxRetries: 3,
          gasMultiplier: 1.3, // 30% safety margin for voting
          extraParams: {
            // Add any additional parameters if needed
          }
        }
      );
      
      // Check if the transaction was successful
      if (receipt.status) {
        setStatus("✅ Vote cast successfully! Transaction hash: " + receipt.transactionHash.slice(0, 10) + "...");
        // Clear the form
        setVoterId("");
        setCandidate("");
        setBiometricChecked(false);
        setBiometricStatus("");
        
        // Force a page refresh to update the dashboard
        setTimeout(() => {
          window.location.reload();
        }, 2000);
      } else {
        setStatus("❌ Transaction failed. Please try again.");
      }
    } catch (err) {
      setStatus(mapTxError(err, "❌ Failed to cast vote."));
    } finally {
      setLoading(false);
    }
  };

  // Simulate biometric check
  const handleBiometricCheck = () => {
    setShowBiometricModal(true);
    setBiometricStatus("");
  };
  
  const completeBiometric = () => {
    setBiometricChecked(true);
    setShowBiometricModal(false);
    setBiometricStatus("✅ Biometric check passed.");
  };
  
  const cancelBiometric = () => {
    setShowBiometricModal(false);
    setBiometricStatus("❌ Biometric check cancelled.");
  };

  return (
    <div className="min-h-screen bg-blue-50 flex items-center justify-center p-6">
      {web3Loading ? (
        <Loading message="Connecting to blockchain..." fullScreen />
      ) : !isConnected ? (
        <div className="bg-white text-black max-w-md p-6 rounded-2xl shadow-lg w-full text-center">
          <h1 className="text-2xl font-bold mb-4">⚠️ Connection Required</h1>
          <p className="text-gray-600 mb-4">Please connect your wallet to access the voting panel.</p>
          {web3Error && <p className="text-red-600 text-sm">{web3Error}</p>}
        </div>
      ) : !currentElection || Number(currentElection.id) === 0 ? (
        <div className="bg-white text-black max-w-md p-6 rounded-2xl shadow-lg w-full text-center">
          <h1 className="text-2xl font-bold mb-4">⚠️ No Active Election</h1>
          <p className="text-gray-600 mb-4">There is currently no active election for voting.</p>
          <p className="text-sm text-gray-500">Please wait for an admin to create a new election.</p>
        </div>
      ) : (
        <div className="bg-white text-black max-w-md p-6 rounded-2xl shadow-lg w-full">
        <h1 className="text-2xl font-bold mb-4">🗳️ Voter Panel</h1>
          
          {/* Current Election Info */}
          <div className="bg-blue-50 p-3 rounded-lg mb-4">
            <h3 className="font-semibold text-blue-800">{currentElection.name}</h3>
            <p className="text-sm text-blue-600">{currentElection.description}</p>
            <p className="text-xs text-blue-500 mt-1">
                             Election ID: {Number(currentElection.id)}
            </p>
          </div>

        <label className="block mb-2 text-sm font-medium">Voter ID</label>
        <input
          type="text"
          placeholder="Enter your Voter ID"
          value={voterId}
          onChange={(e) => setVoterId(e.target.value)}
          className={`w-full border rounded px-3 py-2 mb-4 ${voterId && voterId.trim().length < 3 ? 'border-red-500' : ''}`}
          disabled={loading}
        />

        <label className="block mb-2 text-sm font-medium">Candidate</label>
        {candidates.length === 0 ? (
          <div className="mb-4 text-red-500 text-sm">No candidates available.</div>
        ) : (
          <select
            value={candidate}
            onChange={e => setCandidate(e.target.value)}
            className="w-full border rounded px-3 py-2 mb-4"
            disabled={loading}
          >
            <option value="">Select a candidate</option>
            {candidates.map((c, i) => (
              <option key={i} value={c}>{c}</option>
            ))}
          </select>
        )}
          
        {/* Biometric Check UI */}
        <button
          onClick={handleBiometricCheck}
          className="w-full px-4 py-2 bg-purple-700 text-white rounded mb-2"
          disabled={biometricChecked}
        >
          {biometricChecked ? 'Biometric Check Complete' : 'Start Biometric Check'}
        </button>
        {biometricStatus && <p className={`text-sm ${biometricStatus.startsWith('✅') ? 'text-green-700' : biometricStatus.startsWith('❌') ? 'text-red-600' : 'text-gray-800'}`}>{biometricStatus}</p>}
          
        {/* Biometric Modal (simulated) */}
        {showBiometricModal && (
          <div className="fixed inset-0 bg-black bg-opacity-40 flex items-center justify-center z-50">
            <div className="bg-white p-6 rounded-lg max-w-sm w-full mx-4">
              <h3 className="text-lg font-semibold mb-4">Biometric Verification</h3>
              <p className="text-gray-600 mb-4">Please complete your biometric verification to proceed with voting.</p>
              <div className="flex space-x-3">
                <button
                  onClick={completeBiometric}
                  className="flex-1 px-4 py-2 bg-green-600 text-white rounded hover:bg-green-700"
                >
                  Complete
                </button>
                <button
                  onClick={cancelBiometric}
                  className="flex-1 px-4 py-2 bg-gray-500 text-white rounded hover:bg-gray-600"
                >
                  Cancel
                </button>
              </div>
            </div>
          </div>
        )}

        {/* Action Buttons */}
        <div className="space-y-3 mb-4">
          <button
            onClick={checkRegistration}
            className="w-full px-4 py-2 bg-blue-600 text-white rounded hover:bg-blue-700 disabled:opacity-50"
            disabled={loading || !voterId.trim()}
          >
            Check Registration
          </button>
          
          <button
            onClick={checkVoteStatus}
            className="w-full px-4 py-2 bg-yellow-600 text-white rounded hover:bg-yellow-700 disabled:opacity-50"
            disabled={loading || !voterId.trim()}
          >
            Check Vote Status
          </button>
          
          <button
            onClick={castVote}
            className="w-full px-4 py-2 bg-green-600 text-white rounded hover:bg-green-700 disabled:opacity-50"
            disabled={loading || !voterId.trim() || !candidate || !biometricChecked}
          >
            {loading ? "Casting Vote..." : "Cast Vote"}
          </button>
        </div>

        {/* Status Display */}
        {status && (
          <div className={`p-3 rounded-lg text-sm ${
            status.startsWith('✅') ? 'bg-green-100 text-green-800' :
            status.startsWith('❌') ? 'bg-red-100 text-red-800' :
            status.startsWith('⚠️') ? 'bg-yellow-100 text-yellow-800' :
            'bg-blue-100 text-blue-800'
          }`}>
            {status}
          </div>
        )}
      </div>
      )}
    </div>
  );
}
